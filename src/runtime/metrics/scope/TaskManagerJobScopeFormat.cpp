#include "TaskManagerJobScopeFormat.hpp"

std::vector<std::string> TaskManagerJobScopeFormat::format_scope(std::shared_ptr<TaskManagerMetricGroup> parent, int job_id, const std::string job_name) {
    std::vector<std::string> templates = copy_template();
    std::vector<std::string> values {parent->get_hostname(), parent->get_task_manager_id(), std::to_string(job_id), job_name};
    bind_variables(templates, values);
    return templates;
}