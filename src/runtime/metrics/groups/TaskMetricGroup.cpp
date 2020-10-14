#include "TaskMetricGroup.hpp"

TaskMetricGroup::TaskMetricGroup(MetricRegistryPtr registry, 
                std::shared_ptr<TaskManagerJobMetricGroup> parent, 
                int vertex_id, 
                int execution_id,
                const std::string task_name,
                int subtask_idx):
                ComponentMetricGroup<TaskManagerJobMetricGroup>(registry, ScopeFormats::get_instance()->get_task_format()->format_scope(parent, vertex_id, execution_id, task_name, subtask_idx), parent),
                m_execution_id(execution_id),
                m_vertex_id(vertex_id),
                m_task_name(task_name),
                m_subtask_index(subtask_idx) {
        m_io_metrics = nullptr;
}

TaskMetricGroup::OperatorMetricGroupPtr TaskMetricGroup::get_or_add_operator(int operator_id, std::string operator_name) {
    std::string key = std::to_string(operator_id) + operator_name;

    std::unique_lock<std::mutex> operators_lock(m_global_mtx);

    if (m_operators.find(key) == m_operators.end()) {
        OperatorMetricGroupPtr cur_operator = std::make_shared<OperatorMetricGroup>(m_registry, inheritable_enable_shared_from_this<TaskMetricGroup>::shared_from_this(), operator_id, operator_name);
        m_operators.insert(std::make_pair(key, cur_operator));
        return cur_operator;
    } else {
        return m_operators[key];
    }
}

void TaskMetricGroup::close() {
    std::unique_lock<std::mutex> lock(m_global_mtx);
    ComponentMetricGroup<TaskManagerJobMetricGroup>::close();

    std::map<std::string, OperatorMetricGroupPtr>::iterator i = m_operators.begin();

    while (i != m_operators.end()) {
        i->second->close();
        ++i;
    }

    lock.unlock();

    m_parent->remove_task_metric_group(m_execution_id);
}