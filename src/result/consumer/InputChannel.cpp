#include "InputChannel.hpp"
std::shared_ptr<spdlog::logger> InputChannel::m_logger = LoggerFactory::get_logger("InputChannel");

InputChannel::InputChannel(std::shared_ptr<InputGate> input_gate, int channel_idx, std::string partition_id,
                std::shared_ptr<ResultPartitionManager> partition_manager,
                CounterPtr bytes_in, CounterPtr buffers_in):
m_input_gate(input_gate),
m_partition_manager(partition_manager),
m_partition_id(partition_id),
m_bytes_in(bytes_in),
m_buffers_in(buffers_in){
    m_channel_info = std::make_shared<InputChannelInfo>(input_gate->get_gate_idx(), channel_idx);
}

/**
 * Let input gate known this input channel is not empty.
 * The input gate will queue this channel into the queue input_channel_with_data
 */
void InputChannel::notify_data_available() {
    m_input_gate->notify_channel_non_empty(shared_from_this());
}

/**
 * Get connection to the subpartition in the upstream task via subpartition view.
 */
void InputChannel::request_subpartition(int subpartition_idx) {
    if (m_subpartition_view == nullptr) {
        // m_subpartition_view = m_partition_manager->create_subpartition_view(m_partition_idx, subpartition_idx, shared_from_this());
        m_subpartition_view = m_partition_manager->create_subpartition_view(m_partition_id, subpartition_idx, shared_from_this());
    }
}

/**
 * Poll the buffer from subpartition view.
 */
std::shared_ptr<BufferAndBacklog> InputChannel::get_next_buffer() {
    if (m_subpartition_view == nullptr) {
        throw std::runtime_error("subpartition view is NULL");
    }

    std::shared_ptr<BufferAndBacklog> next = m_subpartition_view->get_next_buffer();

    if (m_bytes_in != nullptr && m_buffers_in != nullptr && next != nullptr){
        m_buffers_in->inc();
        m_bytes_in->inc(next->get_buffer()->get_max_capacity());
    }

    return next;
}
