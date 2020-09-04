/**
 * The result partition manager keeps track of all currently produced / consumed partitions of a task mananger.
 * For our case, one machine refers to a task manager.
 * 
 * Currently, only support one machine.
 */
#pragma once
#include <map>
#include "ResultPartition.hpp"
#include "ResultSubpartition.hpp"
#include "SubpartitionAvailableListener.hpp"

class ResultPartition;
class SubpartitionAvailableListener;

class ResultPartitionManager
{
private:
    std::map<int, std::shared_ptr<ResultPartition>> m_registered_partitions; // TODO: need synchronize!
public:
    ResultPartitionManager(/* args */) {};
    ~ResultPartitionManager() {};

    /* Register / retrieve ResultPartition */
    void                                    register_result_partition(std::shared_ptr<ResultPartition> partition);
    std::shared_ptr<ResultPartition>        get_result_partition(int partition_idx);


    std::shared_ptr<ResultSubpartitionView> create_subpartition_view(int partition_idx, int subpartition_idx, 
                                                                        std::shared_ptr<SubpartitionAvailableListener> view_reader);
};
