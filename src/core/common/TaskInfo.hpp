/**
 * Encapsulate task specific information: name, index of subtask, parallelism and attempt number.
 */
#pragma once
#include <string>

class TaskInfo
{
private:
    std::string         m_task_name;
    std::string         m_task_name_with_subtasks;
    int                 m_index_of_subtask;
    int                 m_number_of_parallel_subtasks;

public:
    TaskInfo(std::string task_name, std::string task_name_with_subtasks, int index_of_subtask, int number_of_parallel_subtasks):
    m_task_name(task_name), m_task_name_with_subtasks(task_name_with_subtasks), m_index_of_subtask(index_of_subtask),
    m_number_of_parallel_subtasks(number_of_parallel_subtasks) {}

    TaskInfo(std::string task_name, int index_of_subtask, int number_of_parallel_subtasks):
    m_task_name(task_name), m_index_of_subtask(index_of_subtask), m_number_of_parallel_subtasks(number_of_parallel_subtasks) {
        m_task_name_with_subtasks = task_name + " (" + std::to_string(index_of_subtask + 1) + "/" + std::to_string(number_of_parallel_subtasks) + ")";
    }

    /* Properties */
    std::string         get_task_name() { return m_task_name;}
    std::string         get_task_name_with_sub_tasks() {return m_task_name_with_subtasks;}
    int                 get_index_of_subtask() {return m_index_of_subtask;}
    int                 get_number_of_parallel_subtasks() {return m_number_of_parallel_subtasks;}

};

