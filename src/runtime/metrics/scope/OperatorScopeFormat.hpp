/**
 * The scope format for the OperatorMetricGroup.
 */
#pragma once
#include "ScopeFormat.hpp"
#include "TaskScopeFormat.hpp"
#include "TaskMetricGroup.hpp"

class TaskMetricGroup;
class TaskScopeFormat;

class OperatorScopeFormat : public ScopeFormat 
{
public:
    OperatorScopeFormat(std::string format, std::shared_ptr<TaskScopeFormat> parent_format);

    std::vector<std::string> format_scope(std::shared_ptr<TaskMetricGroup> parent, const std::string& operator_id, std::string operator_name);
};
