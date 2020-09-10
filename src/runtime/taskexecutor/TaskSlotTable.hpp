/**
 * Container for multiple TaskSlot instances. Additionally, it maintains multiple indices
 * for fast access to tasks and sets of allocated slots.
 */
#pragma once
#include "Task.hpp"
#include "TaskSlot.hpp"
#include <memory>
#include <map>
#include <set>

class TaskSlotMapping{
private:
    std::shared_ptr<Task>       m_task;
    std::shared_ptr<TaskSlot>   m_task_slot;

public:
    TaskSlotMapping(std::shared_ptr<Task> task, std::shared_ptr<TaskSlot> task_slot):
    m_task(task), m_task_slot(task_slot) {}
    
    /* Properties */
    std::shared_ptr<Task>       get_task() {return m_task;}
    std::shared_ptr<TaskSlot>   get_task_slot() {return m_task_slot;}
};

class TaskSlotTable
{
private:
    int                                                 m_number_slots;
    std::map<int, std::shared_ptr<TaskSlot>>            m_task_slots;
    std::map<int, std::shared_ptr<TaskSlot>>            m_allocated_slots;      // mapping from allocation id to task slot
    std::map<int, std::shared_ptr<TaskSlotMapping>>     m_task_slot_mappings;   // mapping from execution id to task and task slot
    std::map<int, std::set<int>>                        m_slots_per_job;        // mapping from job id to allocated slots for a job

public:
    TaskSlotTable(int number_slots):m_number_slots(number_slots) {}

    bool                                                allocate_slot(int index, int job_id, int allocation_id);
    
    bool                                                add_task(std::shared_ptr<Task> task);

    bool                                                try_mark_slot_active(int job_id, int allocation_id);

    std::shared_ptr<Task>                               get_task(int exeuction_id);

    bool                                                is_slot_free(int index) {return m_task_slots.find(index) == m_task_slots.end();}
};

