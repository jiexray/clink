#include "ResultPartition.hpp"
std::shared_ptr<spdlog::logger> ResultPartition::m_logger = LoggerFactory::get_logger("Result_Partition");

ResultPartition::ResultPartition(std::string owning_task_name, 
                                 int partition_idx,
                                 int number_of_subpartitions,
                                 std::shared_ptr<BufferPool> buffer_pool):
m_owning_task_name(owning_task_name),
m_partition_idx(partition_idx),
m_buffer_pool(buffer_pool),
m_subpartitions(number_of_subpartitions){}

void ResultPartition::setup() {
    for (int i = 0; i < (int)m_subpartitions.size(); i++) {
        m_subpartitions[i] = std::make_shared<ResultSubpartition>(i, shared_from_this());
    }
}

/**
 * Get a BufferBuilder from buffer pool using blocking mode.
 */
std::shared_ptr<BufferBuilder> ResultPartition::get_buffer_builder() {
    SPDLOG_LOGGER_TRACE(m_logger, "Requesting BufferBuilder in blocking way from BufferPool {}", m_buffer_pool->get_buffer_pool_id());
    return m_buffer_pool->request_buffer_builder_blocking();
}

/**
 * Get a BufferBuilder from buffer pool using non-blocking mode.
 */
std::shared_ptr<BufferBuilder> ResultPartition::try_get_buffer_builder() {
    return m_buffer_pool->request_buffer_builder();
}

/**
 * Add a BufferConsumer to the subpartition with given index.
 */
bool ResultPartition::add_buffer_consumer(std::shared_ptr<BufferConsumer> buffer_consumer, int subpartition_idx) {
    get_subpartition(subpartition_idx)->add(buffer_consumer);
}

/**
 * Flush the supartition with given index.
 */
void ResultPartition::flush(int subpartition_idx) {
    get_subpartition(subpartition_idx)->flush();
}

/**
 * Flush all subpartitions.
 */
void ResultPartition::flush_all() {
    for (std::shared_ptr<ResultSubpartition> subpartition : m_subpartitions) {
        subpartition->flush();
    }
}

/**
 * Create a subpartition view for the given subpartition.
 */
std::shared_ptr<ResultSubpartitionView> ResultPartition::create_subpartition_view(
                                                    int subpartition_idx, 
                                                    std::shared_ptr<SubpartitionAvailableListener> available_listener) {
    std::shared_ptr<ResultSubpartition> subpartition = get_subpartition(subpartition_idx);
    return subpartition->create_read_view(available_listener);
}


std::shared_ptr<ResultSubpartition> ResultPartition::get_subpartition(int subpartition_idx) {
    if (subpartition_idx < 0 || subpartition_idx >= (int) m_subpartitions.size()) {
        throw std::invalid_argument("subpartition index [" + std::to_string(subpartition_idx) + 
                                    "] is out of the range, subpartitions size [" + std::to_string(m_subpartitions.size()) + "]");
    }
    return m_subpartitions[subpartition_idx];
}