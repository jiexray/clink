/**
 * A container for component scope formats.
 */
#pragma once
#include "ScopeFormat.hpp"
#include "TaskManagerScopeFormat.hpp"
#include "TaskScopeFormat.hpp"
#include "TaskManagerJobScopeFormat.hpp"
#include "OperatorScopeFormat.hpp"
#include <memory>

class TaskManagerScopeFormat;
class TaskManagerJobScopeFormat;
class TaskScopeFormat;
class OperatorScopeFormat;

class ScopeFormats
{
private:
    typedef std::shared_ptr<TaskManagerScopeFormat> TaskManagerScopeFormatPtr;
    typedef std::shared_ptr<TaskScopeFormat> TaskScopeFormatPtr;
    typedef std::shared_ptr<TaskManagerJobScopeFormat> TaskManagerJobScopeFormatPtr;
    typedef std::shared_ptr<OperatorScopeFormat> OperatorScopeFormatPtr;

    TaskManagerScopeFormatPtr               m_task_manager_format;
    TaskScopeFormatPtr                      m_task_format;
    TaskManagerJobScopeFormatPtr            m_task_manager_job_format;
    OperatorScopeFormatPtr                  m_operator_format;
    
    ScopeFormats(std::string task_manager_format_str, std::string task_format_str, std::string task_manager_job_format_str, std::string operator_format_str);

    ~ScopeFormats() {
        m_instance_ptr = nullptr;
    }

    static ScopeFormats*                    m_instance_ptr;
public:
    
    /* Properties */
    TaskManagerScopeFormatPtr               get_task_manager_format() {return m_task_manager_format;}

    TaskScopeFormatPtr                      get_task_format() {return m_task_format;}

    TaskManagerJobScopeFormatPtr            get_task_manager_job_format() {return m_task_manager_job_format;}

    OperatorScopeFormatPtr                  get_operator_format() {return m_operator_format;}

    static ScopeFormats*                    get_instance();
};

