#include "TaskExecutor.hpp"

std::shared_ptr<spdlog::logger> TaskExecutor::m_logger = LoggerFactory::get_logger("TaskExecutor");

void TaskExecutor::submit_task(std::shared_ptr<TaskDeploymentDescriptor> tdd) {
    // active task slot
    int job_id = tdd->get_job_id();
    int allocation_id = tdd->get_allocation_id();

    if(!m_task_slot_table->try_mark_slot_active(job_id, allocation_id)) {
        throw std::runtime_error("No task slot allocated for job ID" + std::to_string(job_id) + " and allocation ID " + 
                                std::to_string(allocation_id) + ".");
    }

    // transfer ResultPartition and InputGate lists in tdd to vector
    typedef std::vector<std::shared_ptr<ResultPartitionDeploymentDescriptor>> ResultPartitionDeploymentDescriptorList;
    typedef std::vector<std::shared_ptr<InputGateDeploymentDescriptor>> InputGateDeploymentDescriptorList;
    ResultPartitionDeploymentDescriptorList result_partitions; 
    InputGateDeploymentDescriptorList input_gates;
    for (int i = 0; i < tdd->get_number_of_result_partitions(); i++) {
        result_partitions.push_back(tdd->get_result_partitions()[i]);
    }
    for (int i = 0; i < tdd->get_number_of_input_gates(); i++) {
        input_gates.push_back(tdd->get_input_gates()[i]);
    }
    // create BufferPool
    // TODO: One Task has a Network BufferPool (BufferPoolFactory), each ResultPartition has a local BufferPool.
    int total_number_of_subpartitions = 0;

    for (int i = 0; i < tdd->get_number_of_result_partitions(); i++) {
        total_number_of_subpartitions += tdd->get_result_partitions()[i]->get_number_of_subpartitions();
    }
    SPDLOG_LOGGER_INFO(m_logger, "TaskExecutor {}: total subpartitions: {}", m_task_exeuctor_name, total_number_of_subpartitions);
    std::shared_ptr<BufferPool> buffer_pool = std::make_shared<BufferPool>(
        total_number_of_subpartitions * Constant::BUFFER_SUBPARTITION_SCALE, 
        Constant::BUFFER_SIZE);

    // create TaskMetricGroup
    std::shared_ptr<TaskMetricGroup> task_metric_group = m_task_manager_metric_group->add_task_for_job(
        job_id,     
        tdd->get_job_information()->get_job_name(), 
        tdd->get_task_information()->get_job_vertex_id(),  
        tdd->get_execution_id(),
        tdd->get_task_information()->get_task_name(),
        tdd->get_subtask_idx());

    // create task
    std::shared_ptr<Task> task = std::make_shared<Task>(
                                                        tdd->get_job_information(),
                                                        tdd->get_task_information(),
                                                        tdd->get_execution_id(),
                                                        tdd->get_allocation_id(),
                                                        tdd->get_subtask_idx(),
                                                        result_partitions,
                                                        input_gates,
                                                        m_shuffle_environment,
                                                        buffer_pool,
                                                        task_metric_group); 
    // m_logger->info("Receive Task {}", task->get_task_info()->get_task_name_with_sub_tasks());
    SPDLOG_LOGGER_INFO(m_logger, "Receive Task {}", task->get_task_info()->get_task_name_with_sub_tasks());

    // add task to TaskSlotTable
    bool task_added = m_task_slot_table->add_task(task);
    // m_logger->info("Task {} is already submit successfully", task->get_task_info()->get_task_name_with_sub_tasks());
    SPDLOG_LOGGER_INFO(m_logger, "Task {} is already submit successfully", task->get_task_info()->get_task_name_with_sub_tasks());

    // if (task_added) {
    //     task->start_task_thread();

    //     std::cout << "[INFO] task " << task->get_task_info()->get_task_name_with_sub_tasks() << " is already submit successfully" << std::endl;
    // } else {
    //     std::string message = "TaskManager already contains a task for id " + std::to_string(task->get_execution_id()) + ".";
    //     std::cout << "[INFO] "  << message << std::endl;
    //     throw std::runtime_error(message);
    // }    

}

void TaskExecutor::start_task(int execution_id) {
    std::shared_ptr<Task> task = m_task_slot_table->get_task(execution_id);
    if(task == nullptr) {
        SPDLOG_LOGGER_ERROR(m_logger, "Cannot find task with execution_id {}", execution_id);
        throw std::invalid_argument("Cannot find task with execution_id " + std::to_string(execution_id));
    }
    SPDLOG_LOGGER_INFO(m_logger, "Start Task {}", task->get_task_info()->get_task_name_with_sub_tasks());
    task->start_task_thread();
}


void TaskExecutor::cancel_task(int execution_id) {
    std::shared_ptr<Task> task = m_task_slot_table->get_task(execution_id);
    if(task == nullptr) {
        SPDLOG_LOGGER_ERROR(m_logger, "Cannot find task with execution_id {}", execution_id);
        throw std::invalid_argument("Cannot find task with execution_id " + std::to_string(execution_id));
    }
    SPDLOG_LOGGER_INFO(m_logger, "Stop Task {}", task->get_task_info()->get_task_name_with_sub_tasks());
    task->cancel_task();
}


void TaskExecutor::allocate_slot(int slot_id, int job_id, int allocation_id) {
    if (m_task_slot_table->is_slot_free(slot_id)) {
        if (m_task_slot_table->allocate_slot(slot_id, job_id, allocation_id)) {
            SPDLOG_LOGGER_INFO(m_logger, "Allocate slot for allocation_id {}", allocation_id);
        } else {
            SPDLOG_LOGGER_ERROR(m_logger, "Could not allocate slot for allcation_id {}", allocation_id);
            throw std::runtime_error("Could not allocate slot");
        }
    } else {
        std::string message = "The slot " + std::to_string(slot_id) + " has already been allocated or a different job";
        SPDLOG_LOGGER_ERROR(m_logger, message);
        throw std::runtime_error(message);
    }
}