/**
 * The Task represents one execution of a parallel subtasks on a TaskMananger.
 */
#pragma once
#include "TaskInfo.hpp"
#include "Configuration.hpp"
#include "ResultPartition.hpp"
#include "AbstractInvokable.hpp"
#include "executiongraph/TaskInformation.hpp"
#include "executiongraph/JobInformation.hpp"
#include "ResultPartitionDeploymentDescriptor.hpp"
#include "InputGateDeploymentDescriptor.hpp"
#include "ShuffleEnvironment.hpp"
#include "RuntimeEnvironment.hpp"
#include "Constant.hpp"
#include "LoggerFactory.hpp"
#include "TaskMetricGroup.hpp"

#include "OneInputStreamTask.hpp"
#include "SourceStreamTask.hpp"
#include "StreamTaskFactory.hpp"


#include <memory>
#include <string>
#include <thread>
#include <vector>
#include <unistd.h>

class Task
{
private:
    typedef std::vector<std::shared_ptr<ResultPartitionDeploymentDescriptor>> ResultPartitionDeploymentDescriptorList;
    typedef std::vector<std::shared_ptr<InputGateDeploymentDescriptor>> InputGateDeploymentDescriptorList;
    typedef std::shared_ptr<TaskMetricGroup> TaskMetricGroupPtr;
    typedef std::shared_ptr<ShuffleIOOwnerContext> ShuffleIOOwnerContextPtr;
    typedef std::shared_ptr<InputGate> InputGatePtr;

    int                                 m_job_id;
    int                                 m_vertex_id;
    int                                 m_execution_id;
    int                                 m_allocation_id;
    std::shared_ptr<TaskInfo>           m_task_info;
    std::string                         m_task_name_with_subtask;
    std::shared_ptr<Configuration>      m_task_configuration;

    std::shared_ptr<ResultPartition>*   m_result_partitions;
    int                                 m_number_of_result_partitions;
    std::shared_ptr<InputGate>*         m_input_gates;
    int                                 m_number_of_input_gates;

    std::shared_ptr<std::thread>        m_executing_thread;

    std::shared_ptr<AbstractInvokable>  m_invokable;
    std::string                         m_name_of_invokable_class;

    std::shared_ptr<BufferPool>         m_buffer_pool;

    TaskMetricGroupPtr                  m_metrics;

    // logger for local thread
    static std::shared_ptr<spdlog::logger>     m_logger;

public:
    Task(std::shared_ptr<JobInformation> job_information, std::shared_ptr<TaskInformation> task_information,
            int execution_id, int allocation_id, int subtask_idx, 
            ResultPartitionDeploymentDescriptorList & result_partition_descriptors,
            InputGateDeploymentDescriptorList & input_gate_descriptors,
            std::shared_ptr<ShuffleEnvironment> shuffle_environment,
            std::shared_ptr<BufferPool> buffer_pool, TaskMetricGroupPtr metrics);

    void                                        do_run();
    void                                        run() {do_run();}

    void                                        start_task_thread();
    void                                        cancel_task();
    
    static std::shared_ptr<AbstractInvokable>   load_and_instantiate_invokable(std::string class_name, std::shared_ptr<Environment> env);

    /* Properties */
    int                                         get_job_id() {return m_job_id;}
    int                                         get_execution_id() {return m_execution_id;}
    int                                         get_allocation_id() {return m_allocation_id;}
    std::shared_ptr<TaskInfo>                   get_task_info() {return m_task_info;}
    void                                        set_logger(std::shared_ptr<spdlog::logger> logger) {m_logger = logger;}
};


