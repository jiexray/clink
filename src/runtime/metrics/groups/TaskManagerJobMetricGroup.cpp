#include "TaskManagerJobMetricGroup.hpp"

TaskManagerJobMetricGroup::TaskManagerJobMetricGroup(std::shared_ptr<MetricRegistry> registry,
                            std::shared_ptr<TaskManagerMetricGroup> parent,
                            int job_id, 
                            const std::string job_name):
                            JobMetricGroup<TaskManagerMetricGroup>(registry, parent, job_id, job_name, 
                                ScopeFormats::get_instance()->get_task_manager_job_format()->format_scope(parent, job_id, job_name)) {}

TaskManagerJobMetricGroup::TaskMetricGroupPtr TaskManagerJobMetricGroup::add_task(int vertex_id, 
                                                        int execution_id, 
                                                        const std::string task_name, 
                                                        int subtask_index) {
    std::unique_lock<std::mutex> m_tasks_lock(m_global_mtx);
    if (is_closed()) {
        return nullptr;
    }

    if (m_tasks.find(execution_id) != m_tasks.end()) {
        return m_tasks[execution_id];
    }

    TaskMetricGroupPtr task = std::make_shared<TaskMetricGroup>(m_registry, 
                                                                inheritable_enable_shared_from_this<TaskManagerJobMetricGroup>::shared_from_this(),
                                                                vertex_id,
                                                                execution_id,
                                                                task_name,
                                                                subtask_index);
    m_tasks.insert(std::make_pair(execution_id, task));
    return task;
}

void TaskManagerJobMetricGroup::remove_task_metric_group(int execution_id) {
    bool remove_from_parent = false;

    std::unique_lock<std::mutex> m_tasks_lock(m_global_mtx);

    if (m_tasks.find(execution_id) != m_tasks.end()) {
        m_tasks.erase(execution_id);

        if (m_tasks.empty() && !is_closed()) {
            remove_from_parent = true;
            close();
        }
    }

    m_tasks_lock.unlock();

    if (remove_from_parent) {
        m_parent->remove_job_metric_group(m_job_id, inheritable_enable_shared_from_this<TaskManagerJobMetricGroup>::shared_from_this());
    }
}

void TaskManagerJobMetricGroup::close() {
    std::unique_lock<std::mutex> lock(m_global_mtx);
    if (!is_closed()) {
        JobMetricGroup<TaskManagerMetricGroup>::close();

        std::map<int, TaskMetricGroupPtr>::iterator i = m_tasks.begin();
        while (i != m_tasks.end()) {
            i->second->close();
            ++i;
        }
    }
}