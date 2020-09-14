/**
 * The result partition manager keeps track of all currently produced / consumed partitions of a task mananger.
 * For our case, one machine refers to a task manager.
 * 
 * Currently, only support one machine.
 */
#pragma once
#include <map>
#include <algorithm>
#include <iterator>
#include <string>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include "ResultPartition.hpp"
#include "ResultSubpartition.hpp"
#include "SubpartitionAvailableListener.hpp"

class ResultPartition;
class SubpartitionAvailableListener;

class ResultPartitionManager
{
private:
    std::map<std::string, std::shared_ptr<ResultPartition>> m_registered_partitions; // TODO: need synchronize!
    static std::shared_ptr<spdlog::logger>                  m_logger;
public:
    ResultPartitionManager(/* args */) {
        spdlog::set_pattern(Constant::SPDLOG_PATTERN);
        spdlog::set_level(Constant::SPDLOG_LEVEL);
    };
    ~ResultPartitionManager() {};

    // NOTE: (9.9) register result partition with result_partition_id, not partition_idx

    /* Register / retrieve ResultPartition */
    void                                    register_result_partition(std::string task_name, std::shared_ptr<ResultPartition> partition);
    void                                    register_result_partition(std::shared_ptr<ResultPartition> partition);  // for test
    std::shared_ptr<ResultPartition>        get_result_partition(std::string partition_id);
    std::shared_ptr<ResultPartition>        get_result_partition(int partition_idx); // for test


    std::shared_ptr<ResultSubpartitionView> create_subpartition_view(std::string partition_id, int subpartition_idx, 
                                                                        std::shared_ptr<SubpartitionAvailableListener> available_listener);
    std::shared_ptr<ResultSubpartitionView> create_subpartition_view(int partition_idx, int subpartition_idx,       // for test
                                                                        std::shared_ptr<SubpartitionAvailableListener> view_reader);
};
