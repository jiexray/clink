/**
 * Special MetricGroup representing everything belonging to a specific job, running on the TaskManager.
 */
#pragma once
#include "JobMetricGroup.hpp"
#include "TaskManagerMetricGroup.hpp"
#include "TaskMetricGroup.hpp"
#include "MetricRegistry.hpp"
#include <map>
#include <mutex>

class TaskManagerMetricGroup;
class ScopeFormats;
class TaskMetricGroup;

class TaskManagerJobMetricGroup: public JobMetricGroup<TaskManagerMetricGroup>, public inheritable_enable_shared_from_this<TaskManagerJobMetricGroup>
{
private:
    typedef std::shared_ptr<TaskMetricGroup> TaskMetricGroupPtr;
    /* Map from execution_id to task metrics */
    std::map<int, TaskMetricGroupPtr>   m_tasks;
public:
    TaskManagerJobMetricGroup(std::shared_ptr<MetricRegistry> registry,
                            std::shared_ptr<TaskManagerMetricGroup> parent,
                            int job_id, 
                            const std::string job_name);

    std::shared_ptr<TaskManagerMetricGroup> get_parent() {return m_parent;}

    // -------------------------------------------------------------------
    //  adding / removing tasks
    // -------------------------------------------------------------------
    TaskMetricGroupPtr add_task(int vertex_id, int execution_id, const std::string task_name, int subtask_index);

    void remove_task_metric_group(int execution_id);

    // -------------------------------------------------------------------
    //  Component Metric Group Specific
    // -------------------------------------------------------------------
    void close() override;
};

