#include "InputGate.hpp"
std::shared_ptr<spdlog::logger> InputGate::m_logger = LoggerFactory::get_logger("InputGate");

void InputGate::notify_channel_non_empty(std::shared_ptr<InputChannel> channel) {
    queue_channel(channel);  
}

/**
 * Insert channel into channel_queue_with_data if it does not exists.
 */
void InputGate::queue_channel(std::shared_ptr<InputChannel> channel) {
    CompletableFuturePtr to_notify = nullptr;

    std::unique_lock<std::mutex> channels_with_data_lock(m_input_channels_with_data_mtx);

    if (m_enqueued_input_channels_with_data.test(channel->get_channel_idx())) {
        return;
    }

    int available_channels = m_input_channels_with_data.size();

    m_input_channels_with_data.push_back(channel);
    m_enqueued_input_channels_with_data.set(channel->get_channel_idx());

    if (available_channels == 0) {
        available_condition_variable.notify_all();
        SPDLOG_LOGGER_TRACE(m_logger, "InputGate m_availability_helper->get_unavailable_to_reset_available()");
        to_notify = m_availability_helper->get_unavailable_to_reset_available();
    }

    channels_with_data_lock.unlock();

    if (to_notify != nullptr) {
        SPDLOG_LOGGER_TRACE(m_logger, "to_notify->complete(true), InputGate before notify data available");
        to_notify->complete(true);
    }
}

void InputGate::queue_channelV2(std::shared_ptr<InputChannel> channel) {
    CompletableFuturePtr to_notify = nullptr;
    int available_channels = 0;

    std::unique_lock<std::mutex> channels_with_data_lock(m_input_channels_with_data_mtx);

    if (m_enqueued_input_channels_with_data.test(channel->get_channel_idx())) {
        return;
    }
    available_channels = m_enqueued_input_channels_with_data.count();
    m_enqueued_input_channels_with_data.set(channel->get_channel_idx());

    channels_with_data_lock.unlock();

    m_input_channels_with_dataV2.push(channel);

    if (available_channels == 0) {
        to_notify = m_availability_helper->get_unavailable_to_reset_available();
        if (to_notify != nullptr) {
            to_notify->complete(true);
        }
    }
    
}

/**
 * Get a channel from channel_queue_with_data for data polling.
 */
std::shared_ptr<InputChannel> InputGate::get_channel(bool blocking) {
    std::unique_lock<std::mutex> channels_with_data_lock(m_input_channels_with_data_mtx);
    while ((int) m_input_channels_with_data.size() == 0) {
        if (blocking) {
            std::unique_lock<std::mutex> available_helper_lock(available_helper);
            available_condition_variable.wait(available_helper_lock);
        } else {
            SPDLOG_LOGGER_TRACE(m_logger, "InputGate m_availability_helper->reset_unavailable()");
            m_availability_helper->reset_unavailable();
            return nullptr;
        }
        // do not need to get input channel right now, currently only have one thread to poll input channel
    }

    std::shared_ptr<InputChannel> input_channel = m_input_channels_with_data.front();
    m_input_channels_with_data.pop_front();
    m_enqueued_input_channels_with_data.reset(input_channel->get_channel_idx());

    return input_channel;
}

std::shared_ptr<InputChannel> InputGate::get_channelV2(bool blocking) {
    assert(blocking == false);

    std::shared_ptr<InputChannel> input_channel;
    bool has_available_channel = m_input_channels_with_dataV2.try_pop(input_channel);

    if (has_available_channel) {
        std::unique_lock<std::mutex> channels_with_data_lock(m_input_channels_with_data_mtx);
        m_enqueued_input_channels_with_data.reset(input_channel->get_channel_idx());
        channels_with_data_lock.unlock();

        return input_channel;
    } else {
        std::unique_lock<std::mutex> channels_with_data_lock(m_input_channels_with_data_mtx);
        int available_channels = m_enqueued_input_channels_with_data.count();
        channels_with_data_lock.unlock();
        if (available_channels == 0) {
            m_availability_helper->reset_unavailable();
        }
        return nullptr;
    }
}

void InputGate::set_input_channels(std::shared_ptr<InputChannel>* input_channels, int num_input_channels) {
    std::unique_lock<std::mutex> request_lock(m_request_mtx);
    assert(num_input_channels == m_number_of_input_channels);
    for (int i = 0; i < num_input_channels; i++) {
        m_input_channels.insert(std::make_pair(input_channels[i]->get_partition_id(), input_channels[i]));
    }
}


void InputGate::request_partitions() {
    std::unique_lock<std::mutex> request_lock(m_request_mtx);
    int i = 1;
    for (std::pair<std::string, std::shared_ptr<InputChannel>> input_channel: m_input_channels) {
        SPDLOG_LOGGER_INFO(m_logger, "InputGate {} request partitions {}/{}, subpartition_idx: {}", 
            m_owning_task_name, 
            i++,
            m_number_of_input_channels, 
            m_consumed_subpartition_idx);
        input_channel.second->request_subpartition(m_consumed_subpartition_idx);
    } 
}

/**
 * poll buffer from input gate 
 * TODO: Do test.
 */
std::shared_ptr<BufferOrEvent> InputGate::poll_next() {
    while (true) {
        std::shared_ptr<InputChannel> input_channel = get_channel(false);

        if (input_channel == nullptr) {
            return nullptr;
        }

        std::shared_ptr<BufferAndBacklog> result = input_channel->get_next_buffer();

        // put input_channel back to input_channels_with_data, if there is more available data in the input_channel
        { // critical section for getting access to input_channels_with_data
            std::unique_lock<std::mutex> channels_with_data_lock(m_input_channels_with_data_mtx);
            
            if (result != nullptr && result->get_data_available()) {
                m_input_channels_with_data.push_back(input_channel);
                m_enqueued_input_channels_with_data.set(input_channel->get_channel_idx());
            }

            if (m_input_channels_with_data.empty()) {
                SPDLOG_LOGGER_TRACE(m_logger, "InputGate::poll_next(), after get buffer from channel set input_gate unavailable");
                m_availability_helper->reset_unavailable();
            }

            if (result != nullptr) {
                return update_metrics(
                    std::make_shared<BufferOrEvent>(result->get_buffer(), input_channel->get_channel_idx(), !m_input_channels_with_data.empty()));
            }
        }
    }
}

std::shared_ptr<BufferOrEvent> InputGate::poll_nextV2() {
    while (true) {
        std::shared_ptr<InputChannel> input_channel = get_channelV2(false);

        if (input_channel == nullptr) {
            return nullptr;
        }
        
        std::shared_ptr<BufferAndBacklog> result = input_channel->get_next_buffer();
        
        if (result != nullptr && result->get_data_available()) {
            std::unique_lock<std::mutex> channels_with_data_lock(m_input_channels_with_data_mtx);
            m_enqueued_input_channels_with_data.set(input_channel->get_channel_idx());
            channels_with_data_lock.unlock();

            m_input_channels_with_dataV2.push(input_channel);
        }

        std::unique_lock<std::mutex> channels_with_data_lock(m_input_channels_with_data_mtx);
        int available_channels = m_enqueued_input_channels_with_data.count();
        channels_with_data_lock.unlock();
        if (available_channels == 0) {
            m_availability_helper->reset_unavailable();
        }

        if (result != nullptr) {
            return update_metrics(
                    std::make_shared<BufferOrEvent>(result->get_buffer(), input_channel->get_channel_idx(), !m_input_channels_with_data.empty()));
        }
    }
    
}