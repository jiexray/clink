/**
 * The Environment gives the code executed in a task access to the task's properties
 * (such as name, parallelism), the configurations, the data stream readers and writers,
 * as well as the various components that are provided by the TaskManager, such as
 * memory manager, I/O manager, ...
 */
#pragma once
#include "../result/ResultPartition.hpp"
#include "../result/consumer/InputGate.hpp"
#include "../core/config/Configuration.hpp"
#include "../core/common/TaskInfo.hpp"

class Configuration;

class Environment
{
private:
public:
    virtual int                                 get_job_id() = 0;
    virtual int                                 get_job_vertex_id() = 0;
    virtual int                                 get_execution_id() = 0;

    // Fields relevent to the I/O system. Should go into Task
    virtual std::shared_ptr<ResultPartition>    get_writer(int idx) = 0;
    virtual std::shared_ptr<InputGate>          get_input_gate(int idx) = 0;
    virtual std::shared_ptr<Configuration>      get_task_configuration() = 0;
    virtual std::shared_ptr<TaskInfo>           get_task_info() = 0;
};

