/**
 * The scope format for the JobMetricGroup.
 */
#pragma once

#include "ScopeFormat.hpp"
#include "TaskManagerScopeFormat.hpp"
#include "TaskManagerMetricGroup.hpp"

class TaskManagerMetricGroup;

class TaskManagerJobScopeFormat: public ScopeFormat
{
public:
    TaskManagerJobScopeFormat(std::string format, std::shared_ptr<TaskManagerScopeFormat> parent_format):
    ScopeFormat(format, parent_format, std::vector<std::string>{SCOPE_HOST, SCOPE_TASKMANAGER_ID, SCOPE_JOB_ID, SCOPE_JOB_NAME}) {}

    std::vector<std::string> format_scope(std::shared_ptr<TaskManagerMetricGroup> parent, int job_id, const std::string job_name);
};
