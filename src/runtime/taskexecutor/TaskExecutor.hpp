/**
 * Task executor on a machine, which is responsible for the execution of multiple Task.
 */
#pragma once
#include <string>
#include <memory>
#include <vector>
#include "TaskSlotTable.hpp"
#include "TaskDeploymentDescriptor.hpp"


class TaskExecutor
{
private:
    std::shared_ptr<TaskSlotTable>          m_task_slot_table;
    std::shared_ptr<ShuffleEnvironment>     m_shuffle_environment;
public:
    TaskExecutor(std::shared_ptr<TaskSlotTable> task_slot_table, std::shared_ptr<ShuffleEnvironment> shuffle_environment): 
    m_task_slot_table(task_slot_table), m_shuffle_environment(shuffle_environment){}

    void        submit_task(std::shared_ptr<TaskDeploymentDescriptor> tdd);

    void        cancel_task(int execution_id);
    
    void        allocate_slot(int slot_id, int job_id, int allocation_id);
};
