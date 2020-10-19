/**
 * Implementation of the Environment.
 */
#pragma once
#include "Environment.hpp"

class RuntimeEnvironment : public Environment{ 
private:
    typedef std::shared_ptr<TaskMetricGroup> TaskMetricGroupPtr;

    int                                     m_job_id;
    int                                     m_job_vertex_id;
    int                                     m_execution_id;

    std::shared_ptr<ResultPartition>*       m_result_partitions;
    int                                     m_num_of_result_partitions;
    std::shared_ptr<InputGate>*             m_input_gates;
    int                                     m_num_of_input_gates;
    std::shared_ptr<Configuration>          m_task_configuration;
    std::shared_ptr<TaskInfo>               m_task_info;
    TaskMetricGroupPtr                      m_metrics;

public:
    RuntimeEnvironment(int job_id, int job_vertex_id, int execution_id, 
                        std::shared_ptr<ResultPartition>*   result_partitions, 
                        int                                 num_of_result_partitions,
                        std::shared_ptr<InputGate>*         input_gates,
                        int                                 num_of_input_gates,
                        std::shared_ptr<Configuration>      task_configuration,
                        std::shared_ptr<TaskInfo>           task_info) {
            throw std::runtime_error("RuntimeEnvironment need TaskMetricGroup, this API is depercated");
        }


    RuntimeEnvironment(int job_id, int job_vertex_id, int execution_id, 
                        std::shared_ptr<ResultPartition>*   result_partitions, 
                        int                                 num_of_result_partitions,
                        std::shared_ptr<InputGate>*         input_gates,
                        int                                 num_of_input_gates,
                        std::shared_ptr<Configuration>      task_configuration,
                        std::shared_ptr<TaskInfo>           task_info,
                        TaskMetricGroupPtr                  metrics) {
            this->m_job_id                          = job_id;
            this->m_job_vertex_id                   = job_vertex_id;
            this->m_execution_id                    = execution_id;
            this->m_result_partitions               = result_partitions;
            this->m_num_of_result_partitions        = num_of_result_partitions;
            this->m_input_gates                     = input_gates;
            this->m_num_of_input_gates              = num_of_input_gates;
            this->m_task_configuration              = task_configuration;
            this->m_task_info                       = task_info;
            this->m_metrics                         = metrics;
        }

    int                                     get_job_id() {return m_job_id;}
    int                                     get_job_vertex_id() {return m_job_vertex_id;}
    int                                     get_execution_id() {return m_execution_id;}

    // Fields relevent to the I/O system. Should go into Task
    std::shared_ptr<ResultPartition>        get_writer(int idx) {return m_result_partitions[idx];}
    std::shared_ptr<InputGate>              get_input_gate(int idx) {return m_input_gates == nullptr ? nullptr : m_input_gates[idx];}
    std::shared_ptr<Configuration>          get_task_configuration() {return m_task_configuration;}
    std::shared_ptr<TaskInfo>               get_task_info() {return m_task_info;}
    std::shared_ptr<TaskMetricGroup>        get_metric_group() {return m_metrics;}
};
