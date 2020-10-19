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
#include "LoggerFactory.hpp"
#include "TaskManagerMetricGroup.hpp"
#include "TaskManagerServices.hpp"

#include <unistd.h>


class TaskExecutor
{
private:
    typedef std::shared_ptr<TaskManagerMetricGroup> TaskManagerMetricGroupPtr;
    typedef std::shared_ptr<TaskManagerServices>    TaskManagerServicesPtr;

    std::string                                 m_task_exeuctor_name;
    std::shared_ptr<TaskSlotTable>              m_task_slot_table;
    std::shared_ptr<ShuffleEnvironment>         m_shuffle_environment;
    TaskManagerMetricGroupPtr                   m_task_manager_metric_group;

    static std::shared_ptr<spdlog::logger>      m_logger;
public:

    TaskExecutor(TaskManagerServicesPtr task_service_manager, TaskManagerMetricGroupPtr task_manager_metric_group):
    m_task_slot_table(task_service_manager->get_task_slot_table()),
    m_shuffle_environment(task_service_manager->get_shuffle_environment()),
    m_task_manager_metric_group(task_manager_metric_group) {
        m_task_exeuctor_name = "taskexecutor-" + std::to_string(getpid());
    }

    TaskExecutor(std::shared_ptr<TaskSlotTable> task_slot_table, 
                 std::shared_ptr<ShuffleEnvironment> shuffle_environment): 
    m_task_slot_table(task_slot_table), m_shuffle_environment(shuffle_environment),
    m_task_manager_metric_group(nullptr){
        // initialize the logger of TaskExecutor, this logger will be shared among all task in this TaskExecutor.
        // m_task_exeuctor_name = "taskexecutor-" + std::to_string(getpid());
        throw std::runtime_error("TaskExecutor::ctor() need TaskExecutorRunner, this API is depercated");
    }

    void        submit_task(std::shared_ptr<TaskDeploymentDescriptor> tdd);

    void        start_task(int execution_id);
    void        cancel_task(int execution_id);
    
    void        allocate_slot(int slot_id, int job_id, int allocation_id);
};
