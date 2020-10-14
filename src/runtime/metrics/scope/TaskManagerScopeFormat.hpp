/**
 * The scope format for the TaskManagerMetricGroup.
 */
#pragma once
#include "ScopeFormat.hpp"
#include <vector>

class TaskManagerScopeFormat : public ScopeFormat
{
public:
    TaskManagerScopeFormat(std::string format): ScopeFormat(format, nullptr, std::vector<std::string>{SCOPE_HOST, SCOPE_TASKMANAGER_ID}) {}

    std::vector<std::string> format_scope(const std::string& host, const std::string& task_manager_id) {
        std::vector<std::string> templates = copy_template();
        std::vector<std::string> values{host, task_manager_id};
        bind_variables(templates, values);
        return templates;
    }
};

