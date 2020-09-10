#include "TaskSlotTable.hpp"


bool TaskSlotTable::allocate_slot(int index, int job_id, int allocation_id) {
    if (index > m_number_slots) {
        throw std::invalid_argument("index " + std::to_string(index) + " is greater than number of slots " + std::to_string(m_number_slots));
    }

    if (m_allocated_slots.find(allocation_id) != m_allocated_slots.end()) {
        std::cout << "[INFO] Allocation ID " << allocation_id << " is already allocated in " 
            << m_allocated_slots[allocation_id]->to_string() << std::endl;
        return false;
    }

    if (m_task_slots.find(index) != m_task_slots.end()) {
        std::shared_ptr<TaskSlot> task_slot = m_task_slots[index];
        std::cout << "[INFO] Slot with index " << index 
                << " has already exist, with job id " << task_slot->get_job_id() 
                << " and allocation id " << task_slot->get_allocate_id() << std::endl;
        return (task_slot->get_allocate_id() == allocation_id) && (task_slot->get_job_id() == job_id);
    }

    std::shared_ptr<TaskSlot> task_slot = std::make_shared<TaskSlot>(index, job_id, allocation_id);

    m_task_slots.insert(std::make_pair(index, task_slot));

    // update the allocation id to task slot map
    m_allocated_slots.insert(std::make_pair(allocation_id, task_slot));

    // add this slot to the set of job slots
    if (m_slots_per_job.find(job_id) == m_slots_per_job.end()){
        std::set<int> slots;
        m_slots_per_job.insert(std::make_pair(job_id, slots));
    }
    m_slots_per_job[job_id].insert(allocation_id);
    
    return true;
}

bool TaskSlotTable::add_task(std::shared_ptr<Task> task) {
    if(m_allocated_slots.find(task->get_allocation_id()) == m_allocated_slots.end()) {
        throw std::runtime_error("Slot not found");
    }
    std::shared_ptr<TaskSlot> task_slot = m_allocated_slots[task->get_allocation_id()];

    if (task_slot->add(task)) {
        m_task_slot_mappings.insert(std::make_pair(task->get_execution_id(), std::make_shared<TaskSlotMapping>(task, task_slot)));
        return true;
    } else {
        return false;
    }
}

/**
 * Try to mark the specified slot as active if it has veen allocated by the given job.
 */
bool TaskSlotTable::try_mark_slot_active(int job_id, int allocation_id) {
    if (m_allocated_slots.find(allocation_id) != m_allocated_slots.end()) {
        std::shared_ptr<TaskSlot> task_slot = m_allocated_slots[allocation_id];
        return task_slot->mark_active();
    } else {
        return false;
    }
}

/**
 * Get the task for the given execution id.
 */
std::shared_ptr<Task> TaskSlotTable::get_task(int exeuction_id) {
    if (m_task_slot_mappings.find(exeuction_id) != m_task_slot_mappings.end()) {
        std::shared_ptr<TaskSlotMapping> task_slot_mapping = m_task_slot_mappings[exeuction_id];
        return task_slot_mapping->get_task();
    } else {
        return nullptr;
    }
}
