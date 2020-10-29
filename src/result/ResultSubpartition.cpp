#include "ResultSubpartition.hpp"

std::shared_ptr<spdlog::logger> ResultSubpartition::m_logger = LoggerFactory::get_logger("ResultSubpartition");

ResultSubpartition::ResultSubpartition(int index, std::shared_ptr<ResultPartition> parent):
m_parent(parent){
    m_subpartition_info = std::make_shared<ResultSubpartitionInfo>(parent->get_partition_idx(), index);
    m_flush_requested = false;
    m_read_view = nullptr;
}

/**
 * Release the subpartition resources.
 */
void ResultSubpartition::release() {
    // TODO: release the resource of the subpartition.
}

/**
 * Add a buffer comsumer to the subpartition. This will be polled by the read_view later from the
 * downstream ResultReader.
 */
bool ResultSubpartition::add(std::shared_ptr<BufferConsumer> buffer_consumer) {
    // there is at least two threads get access to m_buffers in this subpartition.
    // One is the local task, which will append new buffer consumers to this subpartition.
    // The other one is the downstream task's input channel, which will poll the buffer consumers 
    // for data input.

    bool is_notify_data_available;
    std::unique_lock<std::mutex> buffers_lock(m_buffers_mtx);

    m_buffers.push_back(buffer_consumer);

    is_notify_data_available = should_notify_data_available();

    buffers_lock.unlock();

    if (is_notify_data_available) {
        notify_data_available();
    }

    return true;
}

/**
 * Get a BufferConsumer from the m_buffers, and build the BufferConsumer.
 * Need to kick the BufferConsumer if it has already finish writing.
 * We also need to recycle the buffer.
 */
std::shared_ptr<BufferAndBacklog> ResultSubpartition::poll_buffer() {
    std::unique_lock<std::mutex> buffers_lock(m_buffers_mtx);

    std::shared_ptr<BufferBase> buffer = nullptr;

    if (m_buffers.empty()) {
        m_flush_requested = false;
    }

    while (!m_buffers.empty()) {
        std::shared_ptr<BufferConsumer> buffer_consumer = m_buffers.front();

        buffer = buffer_consumer->build();

        // maybe there is only one buffer in the queue, and it is unfinished.
        // Current data polling is caused by flusher in result partition.
        if (!buffer_consumer->is_finished() && (int) m_buffers.size() != 1) {
            throw std::runtime_error("When there are multiple buffers, an unfinished bufferConsumer can not be at the head of the buffers queue");
        }

        if ((int) m_buffers.size() == 1) {
            // turn off m_flush_requested flag if we have drained all of the available data.
            m_flush_requested = false;
        }

        if (buffer_consumer->is_finished()) {
            m_buffers.pop_front();
            // the buffer_consumer will be destroyed very soon, check its ref count
            if (buffer_consumer.use_count() != 1) {
                throw std::runtime_error("buffer_consumer's use count is not 1, may be used somewhere else");
            }
        }

        if (buffer != nullptr) {
            break;
        }

        if (!buffer_consumer->is_finished()) {
            break;
        }
        // the first buffer consumer is finished, and no buffer has polled, go to the second buffer right now.
    }

    if (buffer == nullptr) {
        return nullptr;
    }

    return std::make_shared<BufferAndBacklog>(buffer, m_flush_requested || get_number_of_finished_buffers() > 0, get_buffers_in_backlog());
}

/**
 * Create a read view, which is a hook for the downstream ResultReader to poll the data
 * from this subpartition.
 */
std::shared_ptr<ResultSubpartitionView> ResultSubpartition::create_read_view(std::shared_ptr<SubpartitionAvailableListener> available_listener) {
    std::unique_lock<std::mutex> buffers_lock(m_buffers_mtx);
    bool is_notify;
    m_read_view = std::make_shared<ResultSubpartitionView>(shared_from_this(), available_listener);

    is_notify = !m_buffers.empty();
    buffers_lock.unlock();

    // When creating the read view, we check the buffer queue. If there is something in the buffer queue, we 
    // proacive flush. For it may wait for flushing.
    if (is_notify) {
        SPDLOG_LOGGER_DEBUG(m_logger, "notify_data_available when creating read view of result paritition in Task {}", m_parent->get_owning_task_name());
        notify_data_available();
    }

    return m_read_view;
}

/**
 * Flush the subpartition, basically, if there is data available, notify the downstream ResultReader
 * to poll the data.
 */
void ResultSubpartition::flush() {
    std::unique_lock<std::mutex> buffers_lock(m_buffers_mtx);
    if ((int) m_buffers.size() == 0 || m_flush_requested) {
        return;
    }

    bool is_notify_data_available = (int) m_buffers.size() == 1 && m_buffers.front()->is_data_available();
    m_flush_requested = m_buffers.size() > 1 || is_notify_data_available;

    buffers_lock.unlock();

    if (is_notify_data_available) {
        notify_data_available();
    }
}
/**
 * Notify only when we added first finished buffer. Or there is two buffers.
 * Note: This function must hold the m_buffer_mtx!
 */
bool ResultSubpartition::should_notify_data_available() {
    if (m_flush_requested || m_read_view == nullptr) {
        // has already been starting a flush
        return false;
    }

    // we only need to notify the read_view when the second buffer consumer is added to the buffer queue.
    // for consequent buffer, no need to notify.
    return get_number_of_finished_buffers() == 1;
}


/**
 *  Note: This function must hold the m_buffer_mtx!
 */
int ResultSubpartition::get_number_of_finished_buffers() {
    if (m_buffers.size() == 1 && m_buffers.front()->is_finished()) {
        // There is only one buffer, and it has already finished, flush it!
        return 1;
    }

    // we assume the last buffer is not finished as default
    return std::max(0, (int) m_buffers.size() - 1);
}

/**
 *  Note: This function must hold the m_buffer_mtx!
 */
int ResultSubpartition::get_buffers_in_backlog() {
    if (m_flush_requested) {
        return (int) m_buffers.size();
    } else {
        return std::max(0, (int) m_buffers.size() - 1);
    }
}
 

void ResultSubpartition::notify_data_available() {
    // if (m_read_view == nullptr) {
    //     std::string owner_name = this->m_parent->get_owning_task_name();
    //     std::cout << "[INFO] Task " << owner_name << " has no registered downstream input channel, m_read_view is null" << std::endl;
    //     throw std::runtime_error("no registered downstream input channel, m_read_view is null");
    // }
    if (m_read_view != nullptr) {
        m_read_view->notify_data_available();
    }
    // std::cout << "m_read_view->notify_data_available()" << std::endl;
}