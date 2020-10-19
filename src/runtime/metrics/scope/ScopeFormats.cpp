#include "ScopeFormats.hpp"

ScopeFormats* ScopeFormats::m_instance_ptr = 0;

ScopeFormats::ScopeFormats(std::string task_manager_format_str, std::string task_manager_job_format_str, std::string task_format_str, std::string operator_format_str) {
    m_task_manager_format = std::make_shared<TaskManagerScopeFormat>(task_manager_format_str);
    m_task_manager_job_format = std::make_shared<TaskManagerJobScopeFormat>(task_manager_job_format_str, m_task_manager_format);
    m_task_format = std::make_shared<TaskScopeFormat>(task_format_str, m_task_manager_job_format);
    m_operator_format = std::make_shared<OperatorScopeFormat>(operator_format_str, m_task_format);
}

ScopeFormats* ScopeFormats::get_instance() {
    // TODO: add config to Configuration class
    std::string tm_format = "<host>.taskmanager.<tm_id>";
    std::string task_format = "<host>.taskmanager.<tm_id>.<job_name>.<task_name>.<subtask_index>";
    std::string task_manager_job_format = "<host>.taskmanager.<tm_id>.<job_name>";
    // TODO: add unique operator_name and operator-id, and omit <task-name> and <subtask_index> here
    std::string operator_format = "<host>.taskmanager.<tm_id>.<job_name>.<task_name>.<subtask_index>.<operator_name>.<subtask_index>";

    if (!m_instance_ptr) {
        static ScopeFormats the_instance(tm_format, task_manager_job_format, task_format, operator_format);
        m_instance_ptr = &the_instance;
    }
    return m_instance_ptr;
}