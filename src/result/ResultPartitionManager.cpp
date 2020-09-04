#include "ResultPartitionManager.hpp"

/**
 * Register a partition to the map m_registered_partitions. The registration may come from multiple threads,
 * need synchronize the map.
 */
void ResultPartitionManager::register_result_partition(std::shared_ptr<ResultPartition> partition) {
    int partition_idx = partition->get_partition_idx();
    m_registered_partitions.insert(std::make_pair(partition_idx, partition));
}

/**
 * Create a reader view for the subpartition (subpartition_idx) of the partition (partition_idx).
 */ 
std::shared_ptr<ResultSubpartitionView> ResultPartitionManager::create_subpartition_view(int partition_idx, int subpartition_idx, 
                                                        std::shared_ptr<SubpartitionAvailableListener> available_listener) {
    std::shared_ptr<ResultPartition> partition = get_result_partition(partition_idx);
    if (partition == nullptr) {
        throw std::invalid_argument("Result partition [" + std::to_string(partition_idx) + "] does not exist.");
    }
    return partition->create_subpartition_view(subpartition_idx, available_listener);
}


std::shared_ptr<ResultPartition> ResultPartitionManager::get_result_partition (int partition_idx) {
    if (m_registered_partitions.find(partition_idx) == m_registered_partitions.end()) {
        throw std::invalid_argument("No partition with partition index [" + std::to_string(partition_idx) + "]");
    }
    return m_registered_partitions[partition_idx];
}