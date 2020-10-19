#include "OperatorScopeFormat.hpp"

OperatorScopeFormat::OperatorScopeFormat(std::string format, std::shared_ptr<TaskScopeFormat> parent_format):
ScopeFormat(format, parent_format, std::vector<std::string>{
    SCOPE_HOST,
    SCOPE_TASKMANAGER_ID,
    SCOPE_JOB_ID,
    SCOPE_JOB_NAME,
    SCOPE_TASK_VERTEX_ID,
    SCOPE_TASK_EXECUTION_ID,
    SCOPE_TASK_NAME,
    SCOPE_TASK_SUBTASK_INDEX,
    SCOPE_OPERATOR_ID,
    SCOPE_OPERATOR_NAME
}) {}

std::vector<std::string> OperatorScopeFormat::format_scope(std::shared_ptr<TaskMetricGroup> parent, const std::string& operator_id, std::string operator_name) {
    std::vector<std::string> templates = copy_template();
    std::vector<std::string> values{
        parent->get_parent()->get_parent()->get_hostname(),
        parent->get_parent()->get_parent()->get_task_manager_id(),
        std::to_string(parent->get_parent()->get_job_id()),
        parent->get_parent()->get_job_name(),
        std::to_string(parent->get_vertex_id()),
        std::to_string(parent->get_execution_id()),
        parent->get_task_name(),
        std::to_string(parent->get_subtask_index()),
        operator_id,
        operator_name
    };
    bind_variables(templates, values);
    return templates;
}