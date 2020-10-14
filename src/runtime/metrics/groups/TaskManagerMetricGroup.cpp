#include "TaskManagerMetricGroup.hpp"

TaskManagerMetricGroup::TaskManagerMetricGroup(MetricRegistryPtr registry, const std::string& hostname, const std::string& task_manager_id):
    ComponentMetricGroup<TaskManagerMetricGroup>(registry, ScopeFormats::get_instance()->get_task_manager_format()->format_scope(hostname, task_manager_id), nullptr),
    m_hostname(hostname), m_task_manager_id(task_manager_id) {}

TaskManagerMetricGroup::TaskMetricGroupPtr TaskManagerMetricGroup::add_task_for_job(int job_id, 
                                    const std::string job_name, 
                                    int job_vertex_id, 
                                    int exeuction_id, 
                                    const std::string task_name, 
                                    int subtask_idx) {
    std::unique_lock<std::mutex> m_jobs_lock(m_global_mtx);

    TaskManagerJobMetricGroupPtr current_job_group;

    if (m_jobs.find(job_id) == m_jobs.end()) {
        // job_id has not inserted
        current_job_group = std::make_shared<TaskManagerJobMetricGroup>(m_registry, inheritable_enable_shared_from_this<TaskManagerMetricGroup>::shared_from_this(), job_id, job_name);
        m_jobs.insert(std::make_pair(job_id, current_job_group));
    } else {
        current_job_group = m_jobs[job_id];
    }

    m_jobs_lock.unlock();

    TaskMetricGroupPtr task_group = current_job_group->add_task(job_vertex_id, exeuction_id, task_name, subtask_idx);

    if (task_group != nullptr) {
        return task_group;
    } else {
        throw std::runtime_error("Create TaskMetricGroup fail");
    }
}

void TaskManagerMetricGroup::remove_job_metric_group(int job_id, 
                                                        TaskManagerJobMetricGroupPtr group) {
    if (group == nullptr) {
        return;
    }

    std::unique_lock<std::mutex> m_jobs_lock(m_global_mtx);
    if (m_jobs.find(job_id) != m_jobs.end()) {
        if (m_jobs[job_id] == group) {
            m_jobs.erase(job_id);
        }
    }
}

void TaskManagerMetricGroup::close() {
    std::unique_lock<std::mutex> lock(m_global_mtx);
    if (!is_closed()){
        ComponentMetricGroup<TaskManagerMetricGroup>::close();

        std::map<int, TaskManagerJobMetricGroupPtr>::iterator i = m_jobs.begin();

        while (i != m_jobs.end()) {
            i->second->close();
            ++i;
        }
    }
}

