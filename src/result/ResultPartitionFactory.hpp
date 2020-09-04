/**
 * Factory for ResultPartition.
 */
#pragma once

#include <memory>
#include <vector>
#include <utility>
#include "ResultPartitionManager.hpp"
#include "ResultPartition.hpp"
#include "ResultSubpartition.hpp"
#include "../deployment/ResultPartitionDeploymentDescriptor.hpp"

class ResultPartitionFactory
{
private:
    std::shared_ptr<ResultPartitionManager>     m_result_partition_manager;
public:
    ResultPartitionFactory(std::shared_ptr<ResultPartitionManager>);
    ~ResultPartitionFactory() {};

    std::shared_ptr<ResultPartition> create(std::string task_name, int partition_idx, 
                                            std::shared_ptr<ResultPartitionDeploymentDescriptor> desc,
                                            std::shared_ptr<BufferPool> buffer_pool);
};

inline ResultPartitionFactory::ResultPartitionFactory(std::shared_ptr<ResultPartitionManager> result_partition_mananger):
m_result_partition_manager(result_partition_mananger)
{}

/**
 * Create a ResultPartition with task name, partition index, ResultPartitionDeploymentDescriptor (future use, currently only number
 * of subpartitions), and the buffer pool.
 * 1. create fake array of subpartitions with number_of_subpartitions;
 * 2. create ResultPartition, with task_name, partition_idx, subpartitions array, buffer_pool, and result_partition_manager;
 * 3. instantiate each subpartition in the fake array of subpartitions;
 * 4. register the ResultPartition to partition mananger;
 */
inline std::shared_ptr<ResultPartition> ResultPartitionFactory::create(
                                            std::string task_name, int partition_idx, 
                                            std::shared_ptr<ResultPartitionDeploymentDescriptor> desc,
                                            std::shared_ptr<BufferPool> buffer_pool) {
    int number_of_subpartitions = desc->get_number_of_subpartitions();
    std::shared_ptr<ResultPartition> partition = std::make_shared<ResultPartition>(task_name, partition_idx, number_of_subpartitions, buffer_pool);
    partition->setup();

    m_result_partition_manager->register_result_partition(partition);

    return partition;    
}
