#include "TaskScopeFormat.hpp"

TaskScopeFormat::TaskScopeFormat(std::string format, std::shared_ptr<TaskManagerJobScopeFormat> parent_format):
ScopeFormat(format, parent_format, std::vector<std::string>{
    SCOPE_HOST, SCOPE_TASKMANAGER_ID, SCOPE_JOB_ID, SCOPE_JOB_NAME, SCOPE_TASK_VERTEX_ID, SCOPE_TASK_EXECUTION_ID, SCOPE_TASK_NAME, SCOPE_TASK_SUBTASK_INDEX
}){}

std::vector<std::string> TaskScopeFormat::format_scope(std::shared_ptr<TaskManagerJobMetricGroup> parent, 
                                            int vertex_id, int execution_id, 
                                            const std::string task_name, int subtask) {
    std::vector<std::string> templates = copy_template();
    std::vector<std::string> values{
        parent->get_parent()->get_hostname(),
        parent->get_parent()->get_task_manager_id(),
        std::to_string(parent->get_job_id()),
        parent->get_job_name(),
        std::to_string(vertex_id),
        std::to_string(execution_id),
        task_name,
        std::to_string(subtask)
    };
    bind_variables(templates, values);
    return templates;
}