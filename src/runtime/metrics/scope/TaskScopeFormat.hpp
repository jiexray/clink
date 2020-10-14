/**
 * The scope format for TaskMetricGroup.
 */
#pragma once
#include "ScopeFormat.hpp"
#include "TaskManagerJobScopeFormat.hpp"
#include "TaskManagerJobMetricGroup.hpp"

class TaskManagerJobMetricGroup;
class TaskManagerJobScopeFormat;

class TaskScopeFormat: public ScopeFormat
{
public:
    TaskScopeFormat(std::string format, std::shared_ptr<TaskManagerJobScopeFormat> parent_format);

    std::vector<std::string> format_scope(std::shared_ptr<TaskManagerJobMetricGroup> parent, 
                                            int vertex_id, int execution_id, 
                                            const std::string task_name, int subtask);
};

