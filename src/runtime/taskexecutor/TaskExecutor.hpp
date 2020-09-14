/**
 * Task executor on a machine, which is responsible for the execution of multiple Task.
 */
#pragma once
#include <string>
#include <memory>
#include <vector>
#include <cstdlib>
#include <time.h>
#include "TaskSlotTable.hpp"
#include "TaskDeploymentDescriptor.hpp"
#include "Constant.hpp"

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

#include <unistd.h>


class TaskExecutor
{
private:
    std::string                                 m_task_exeuctor_name;
    std::shared_ptr<TaskSlotTable>              m_task_slot_table;
    std::shared_ptr<ShuffleEnvironment>         m_shuffle_environment;

    static std::shared_ptr<spdlog::logger>      m_logger;
public:
    TaskExecutor(std::shared_ptr<TaskSlotTable> task_slot_table, std::shared_ptr<ShuffleEnvironment> shuffle_environment): 
    m_task_slot_table(task_slot_table), m_shuffle_environment(shuffle_environment){
        // initialize the logger of TaskExecutor, this logger will be shared among all task in this TaskExecutor.
        m_task_exeuctor_name = "taskexecutor-" + std::to_string(getpid());
        spdlog::set_pattern(Constant::SPDLOG_PATTERN);
        spdlog::set_level(Constant::SPDLOG_LEVEL);
    }

    void        submit_task(std::shared_ptr<TaskDeploymentDescriptor> tdd);

    void        start_task(int execution_id);
    void        cancel_task(int execution_id);
    
    void        allocate_slot(int slot_id, int job_id, int allocation_id);
};
