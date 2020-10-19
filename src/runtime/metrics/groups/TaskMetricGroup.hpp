/**
 * Special MetricGroup representing a runtime Task.
 */
#pragma once
#include "ComponentMetricGroup.hpp"
#include "TaskManagerJobMetricGroup.hpp"
#include "OperatorMetricGroup.hpp"
#include "TaskIOMetricGroup.hpp"
#include <memory>
#include <mutex>

class TaskManagerJobMetricGroup;
class TaskMetricGroup;
class TaskIOMetricGroup;
class OperatorMetricGroup;

class TaskMetricGroup: public ComponentMetricGroup<TaskManagerJobMetricGroup>, public inheritable_enable_shared_from_this<TaskMetricGroup>
{
private:
    typedef std::shared_ptr<OperatorMetricGroup>                OperatorMetricGroupPtr;
    typedef std::shared_ptr<MetricRegistry>                     MetricRegistryPtr;

    std::map<std::string, OperatorMetricGroupPtr>               m_operators;

    std::shared_ptr<TaskIOMetricGroup>                          m_io_metrics;

    int                                                         m_execution_id;
    int                                                         m_vertex_id;
    std::string                                                 m_task_name;
    int                                                         m_subtask_index;
    
public:
    TaskMetricGroup(MetricRegistryPtr registry, 
                    std::shared_ptr<TaskManagerJobMetricGroup> parent, 
                    int vertex_id, 
                    int execution_id,
                    const std::string task_name,
                    int subtask_idx);


    /* Properties */
    std::shared_ptr<TaskManagerJobMetricGroup>              get_parent() {return m_parent;}
    int                                                     get_execution_id() {return m_execution_id;}
    int                                                     get_vertex_id() {return m_vertex_id;}
    std::string                                             get_task_name() {return m_task_name;}
    int                                                     get_subtask_index() {return m_subtask_index;}

    /**
     * Note: get_IO_metric_group() will craete TaskIOMetricGroup if it is null (use global lock).
     */
    std::shared_ptr<TaskIOMetricGroup>                      get_IO_metric_group() {
        if (m_io_metrics == nullptr) {
            m_io_metrics = std::make_shared<TaskIOMetricGroup>(inheritable_enable_shared_from_this<TaskMetricGroup>::shared_from_this());
        }
        return m_io_metrics;
    }

    // -------------------------------------------------------------------------
    //  operators and cleanup
    // -------------------------------------------------------------------------

    OperatorMetricGroupPtr get_or_add_operator(const std::string& operator_id, const std::string& operator_name);

    void close() override;

    // -------------------------------------------------------------------------
    //  Componenet Metric Group Specifics
    // -------------------------------------------------------------------------
    std::string         get_group_name() override {return "task";}

protected:
    void put_variables(std::map<std::string, std::string>& variables) {
        variables.insert(std::make_pair(ScopeFormat::SCOPE_TASK_VERTEX_ID, std::to_string(m_vertex_id)));
        variables.insert(std::make_pair(ScopeFormat::SCOPE_TASK_NAME, m_task_name));
        variables.insert(std::make_pair(ScopeFormat::SCOPE_TASK_EXECUTION_ID, std::to_string(m_execution_id)));
        variables.insert(std::make_pair(ScopeFormat::SCOPE_TASK_SUBTASK_INDEX, std::to_string(m_subtask_index)));
    }
};


