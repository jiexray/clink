/**
 * A task deployment descriptor contains all the information necessary to deploy a task on a task manager.
 */
#pragma once

#include <memory>
#include "InputGateDeploymentDescriptor.hpp"
#include "ResultPartitionDeploymentDescriptor.hpp"
#include "TaskInformation.hpp"
#include "JobInformation.hpp"


class TaskDeploymentDescriptor
{
private:
    int                                     m_job_id;                              // 1
    std::shared_ptr<JobInformation>         m_job_information;                     // 2
    std::shared_ptr<TaskInformation>        m_task_information;                    // 3
    int                                     m_execution_id;                        // 4
    int                                     m_allocation_id;                       // 5
    int                                     m_subtask_idx;                         // 6
    int                                     m_target_slot_number;                  // 7
    int                                     m_number_of_result_partitions;         // 8
    int                                     m_number_of_input_gates;               // 9

    std::shared_ptr<ResultPartitionDeploymentDescriptor>*   m_result_partitions;   // 10
    std::shared_ptr<InputGateDeploymentDescriptor>*         m_input_gates;         // 11

public:
    TaskDeploymentDescriptor(int job_id, std::shared_ptr<JobInformation> job_information,
                            std::shared_ptr<TaskInformation> task_information, 
                            int execution_id, int allocation_id, int subtask_idx,
                            int target_slot_number, 
                            int number_of_result_partitions, int number_of_input_gates,
                            std::shared_ptr<ResultPartitionDeploymentDescriptor>* result_partitions,
                            std::shared_ptr<InputGateDeploymentDescriptor>* input_gates){
        this->m_job_id                          = job_id;
        this->m_job_information                 = job_information;
        this->m_task_information                = task_information;
        this->m_execution_id                    = execution_id;
        this->m_allocation_id                   = allocation_id;
        this->m_subtask_idx                     = subtask_idx;
        this->m_target_slot_number              = target_slot_number;
        this->m_number_of_result_partitions     = number_of_result_partitions;
        this->m_number_of_input_gates           = number_of_input_gates;
        this->m_result_partitions               = result_partitions;
        this->m_input_gates                     = input_gates;
    }

    /* Properties */
    int                                     get_job_id() {return m_job_id;}
    int                                     get_execution_id() {return m_execution_id;}
    int                                     get_allocation_id() {return m_allocation_id;}
    int                                     get_subtask_idx() {return m_subtask_idx;}
    int                                     get_target_slot_number() {return m_target_slot_number;}
    int                                     get_number_of_result_partitions() {return m_number_of_result_partitions;}
    int                                     get_number_of_input_gates() {return m_number_of_input_gates;}

    std::shared_ptr<TaskInformation>        get_task_information() {return m_task_information;}
    std::shared_ptr<JobInformation>         get_job_information() {return m_job_information;}

    std::shared_ptr<ResultPartitionDeploymentDescriptor>*   get_result_partitions() {return m_result_partitions;}
    std::shared_ptr<InputGateDeploymentDescriptor>*         get_input_gates() {return m_input_gates;}

};


