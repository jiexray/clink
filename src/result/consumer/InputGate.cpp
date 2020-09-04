#include "InputGate.hpp"

void InputGate::notify_channel_non_empty(std::shared_ptr<InputChannel> channel) {
    queue_channel(channel);   
}

/**
 * Insert channel into channel_queue_with_data if it does not exists.
 */
void InputGate::queue_channel(std::shared_ptr<InputChannel> channel) {
    std::unique_lock<std::mutex> channels_with_data_lock(m_input_channels_with_data_mtx);

    // TODO: use a bitset to accerlate the input channel searching
    for (std::shared_ptr<InputChannel> in_queue_channel: m_input_channels_with_data) {
        if (channel == in_queue_channel) {
            return;
        }
    }

    int available_channels = m_input_channels_with_data.size();

    m_input_channels_with_data.push_back(channel);

    if (available_channels == 0) {
        available_condition_variable.notify_all();
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
            return nullptr;
        }
        // do not need to get input channel right now, currently only have one thread to poll input channel
    }

    std::shared_ptr<InputChannel> input_channel = m_input_channels_with_data.front();
    m_input_channels_with_data.pop_front();

    return input_channel;
}

void InputGate::set_input_channels(std::shared_ptr<InputChannel>* input_channels, int num_input_channels) {
    std::unique_lock<std::mutex> request_lock(m_request_mtx);
    for (int i = 0; i < num_input_channels; i++) {
        m_input_channels.insert(std::make_pair(input_channels[i]->get_partition_idx(), input_channels[i]));
    }
}


void InputGate::request_partitions() {
    std::unique_lock<std::mutex> request_lock(m_request_mtx);
    for (std::pair<int, std::shared_ptr<InputChannel>> input_channel: m_input_channels) {
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
            }

            if (result != nullptr) {
                return std::make_shared<BufferOrEvent>(result->get_buffer(), input_channel->get_channel_idx(), !m_input_channels_with_data.empty());
            }
        }
    }
}