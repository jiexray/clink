/**
 * Container class for operator/class specific information which are stored at the ExecutionJobVertex.
 * This information is shared by all sub tasks of this operator.
 */
#pragma once
#include <string>
#include "Configuration.hpp"

class TaskInformation
{
private:
    int                                 m_job_vertex_id;
    std::string                         m_task_name;
    int                                 m_number_of_subtasks;
    std::shared_ptr<Configuration>      m_task_configuration;
    std::string                         m_invokable_class_name;
    
public:
    TaskInformation(int job_vertex_id, std::string task_name, int number_of_subtask, std::shared_ptr<Configuration> task_configuration,
                    std::string invokable_class_name):
    m_job_vertex_id(job_vertex_id), m_task_name(task_name), m_number_of_subtasks(number_of_subtask),
    m_task_configuration(task_configuration), m_invokable_class_name(invokable_class_name) {}

    /* Properties */
    int                                 get_job_vertex_id() {return m_job_vertex_id;}
    std::string                         get_task_name() {return m_task_name;}
    int                                 get_number_of_subtasks() {return m_number_of_subtasks;}
    std::shared_ptr<Configuration>      get_task_configuration() {return m_task_configuration;}
    std::string                         get_invokable_class_name() {return m_invokable_class_name;}
};

