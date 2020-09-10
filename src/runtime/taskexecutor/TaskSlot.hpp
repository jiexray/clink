/**
 * Container for multiple Tasks belonging to the same slot.
 */ 
#pragma once
#include "Task.hpp"
#include <map>
#include <string>

enum TaskSlotState {
    ACTIVE, 
    ALLOCATED,
    RELEASING
};

class TaskSlot
{
private:
    int                                     m_index;
    std::map<int, std::shared_ptr<Task>>    m_tasks;
    int                                     m_job_id;
    int                                     m_allocate_id;
    TaskSlotState                           m_state;
public:
    TaskSlot(int index, int job_id, int allocate_id): m_index(index), m_job_id(job_id), m_allocate_id(allocate_id) {
        m_state = TaskSlotState::ALLOCATED;
    }

    /* Properties */
    int                                     get_index() {return m_index;}
    int                                     get_job_id() {return m_job_id;}
    TaskSlotState                           get_state() {return m_state;}
    int                                     get_allocate_id() {return m_allocate_id;}

    bool                                    add(std::shared_ptr<Task> task) {
        if (task->get_job_id() != m_job_id) {
            throw std::runtime_error("Task slot job id " + std::to_string(m_job_id) + " is not equal to task's job id " + std::to_string(task->get_job_id()));
        }
        if (m_tasks.find(task->get_execution_id()) != m_tasks.end()) {
            std::cout << "[INFO] job id " << m_job_id << " with execution id " << task->get_execution_id() << " has already been added in TaskSlot " << m_index << std::endl;
            return false;
        }

        m_tasks.insert(std::make_pair(task->get_execution_id(), task));
        return true;
    }

    bool                                    mark_active() {
        if (m_state == TaskSlotState::ALLOCATED || m_state == TaskSlotState::ACTIVE) {
            m_state = TaskSlotState::ACTIVE;
            return true;
        } else {
            return false;
        }
    }

    std::string                             to_string() {
        return "TaskSlot(index: " + std::to_string(m_index) + 
                ", state:" + std::to_string(m_state) + 
                ", allocationId: " + std::to_string(m_allocate_id) +
                ", jobId: " + std::to_string(m_job_id) + ")";
    }
};

