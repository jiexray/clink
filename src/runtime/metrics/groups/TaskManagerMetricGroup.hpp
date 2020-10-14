/**
 * Special MetricGroup representing a TaskManager
 */
#pragma once
#include "ComponentMetricGroup.hpp"
#include "CollectionUtils.hpp"
#include "TaskManagerJobMetricGroup.hpp"
#include "TaskMetricGroup.hpp"
#include "ScopeFormats.hpp"
#include <map>
#include <vector>
#include <mutex>

class TaskManagerJobMetricGroup;
class TaskMetricGroup;
class ScopeFormats;

class TaskManagerMetricGroup: public ComponentMetricGroup<TaskManagerMetricGroup>, public inheritable_enable_shared_from_this<TaskManagerMetricGroup>
{
private:
    typedef std::shared_ptr<TaskManagerJobMetricGroup>      TaskManagerJobMetricGroupPtr;
    typedef std::shared_ptr<TaskMetricGroup>                TaskMetricGroupPtr;
    typedef std::shared_ptr<MetricRegistry>                 MetricRegistryPtr;

    std::string                                             m_hostname;
    std::string                                             m_task_manager_id;
    std::map<int, TaskManagerJobMetricGroupPtr>             m_jobs;

public:
    TaskManagerMetricGroup(MetricRegistryPtr registry, const std::string& hostname, const std::string& task_manager_id);

    /* Properties */
    std::string get_hostname() {return m_hostname;}
    std::string get_task_manager_id() {return m_task_manager_id;}

    // --------------------------------------------------------------
    //  job groups
    // --------------------------------------------------------------
    TaskMetricGroupPtr                                      add_task_for_job(int job_id, 
                                                                            const std::string job_name, 
                                                                            int job_vertex_id, 
                                                                            int exeuction_id, 
                                                                            const std::string task_name, 
                                                                            int subtask_idx);

    void                                                    remove_job_metric_group(int job_id, 
                                                                            TaskManagerJobMetricGroupPtr group);

    // --------------------------------------------------------------
    //  Component Metric Group Specifics
    // --------------------------------------------------------------
    std::string                                             get_group_name() override {return "taskmanager";}

    void                                                    close() override;

protected:
    void                                                    put_variables(std::map<std::string, std::string>& variables) {
        variables.insert(std::make_pair(ScopeFormat::SCOPE_HOST, m_hostname));
        variables.insert(std::make_pair(ScopeFormat::SCOPE_TASKMANAGER_ID, m_task_manager_id));
    }
};

