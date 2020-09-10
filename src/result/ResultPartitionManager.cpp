#include "ResultPartitionManager.hpp"

/**
 * Register a partition to the map m_registered_partitions. The registration may come from multiple threads,
 * need synchronize the map.
 */
void ResultPartitionManager::register_result_partition(std::string task_name, std::shared_ptr<ResultPartition> partition) {
    std::string partition_id = task_name + "-" + std::to_string(partition->get_partition_idx());
    m_registered_partitions.insert(std::make_pair(partition_id, partition));
}

/* just for test*/
void ResultPartitionManager::register_result_partition(std::shared_ptr<ResultPartition> partition) {
    register_result_partition("test", partition);
}

/**
 * Create a reader view for the subpartition (subpartition_idx) of the partition (partition_id).
 */ 
std::shared_ptr<ResultSubpartitionView> ResultPartitionManager::create_subpartition_view(std::string partition_id, int subpartition_idx, 
                                                        std::shared_ptr<SubpartitionAvailableListener> available_listener) {
    std::cout << "[DEBUG] create subpartition view partition_id " << partition_id << " subpartition_idx " << subpartition_idx << std::endl;
    std::shared_ptr<ResultPartition> partition = get_result_partition(partition_id);
    if (partition == nullptr) {
        throw std::invalid_argument("Result partition [" + partition_id + "] does not exist.");
    }
    return partition->create_subpartition_view(subpartition_idx, available_listener);
}

/**
 * Just for test
 */
std::shared_ptr<ResultSubpartitionView> ResultPartitionManager::create_subpartition_view(int partition_idx, int subpartition_idx, 
                                                        std::shared_ptr<SubpartitionAvailableListener> available_listener) {
    return create_subpartition_view("test-" + std::to_string(partition_idx), subpartition_idx, available_listener);
}


std::shared_ptr<ResultPartition> ResultPartitionManager::get_result_partition (std::string partition_id) {
    if (m_registered_partitions.find(partition_id) == m_registered_partitions.end()) {
        throw std::invalid_argument("No partition with partition index [" + partition_id + "]");
    }
    return m_registered_partitions[partition_id];
}

/**
 * Just for test
 */
std::shared_ptr<ResultPartition> ResultPartitionManager::get_result_partition (int partition_idx) {
    std::string partition_id = "test-" + std::to_string(partition_idx);
    std::cout << "test get_result_partition idx " << partition_idx << ", partition id " << partition_id << std::endl;
    return get_result_partition(partition_id);
}