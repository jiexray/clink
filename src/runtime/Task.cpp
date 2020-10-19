#include "Task.hpp"

std::shared_ptr<spdlog::logger>  Task::m_logger = LoggerFactory::get_logger("Task");

Task::Task(std::shared_ptr<JobInformation> job_information, std::shared_ptr<TaskInformation> task_information,
        int execution_id, int allocation_id, int subtask_idx, 
        ResultPartitionDeploymentDescriptorList & result_partition_descriptors,
        InputGateDeploymentDescriptorList & input_gate_descriptors,
        std::shared_ptr<ShuffleEnvironment> shuffle_environment,
        std::shared_ptr<BufferPool> buffer_pool,
        TaskMetricGroupPtr metrics){
    /* Basic information */
    this->m_job_id                  = job_information->get_job_id();
    this->m_vertex_id               = task_information->get_job_vertex_id();
    this->m_allocation_id           = allocation_id;
    this->m_task_configuration      = task_information->get_task_configuration();
    this->m_execution_id            = execution_id;
    this->m_task_info               = std::make_shared<TaskInfo>(
                                        task_information->get_task_name(),
                                        subtask_idx,
                                        task_information->get_number_of_subtasks());
    this->m_task_name_with_subtask  = m_task_info->get_task_name_with_sub_tasks();
    this->m_name_of_invokable_class = task_information->get_invokable_class_name();
    this->m_buffer_pool             = buffer_pool;
    this->m_executing_thread        = nullptr;

    this->m_metrics                 = metrics;

    // bind TaskMetricGroup to Task's input and output
    ShuffleIOOwnerContextPtr task_shuffle_context = shuffle_environment->create_shuffle_io_owner_context(m_task_name_with_subtask, m_execution_id, metrics->get_IO_metric_group());

    /* Initializer ResultPartition and InputGates */
    this->m_number_of_result_partitions     = (int) result_partition_descriptors.size();
    this->m_result_partitions               = shuffle_environment->create_result_partitions(m_task_name_with_subtask, result_partition_descriptors, m_buffer_pool, task_shuffle_context);

    this->m_number_of_input_gates           = (int) input_gate_descriptors.size();
    this->m_input_gates                     = shuffle_environment->create_input_gates(m_task_name_with_subtask, input_gate_descriptors, task_shuffle_context);
    for (int i = 0; i < m_number_of_input_gates; i++) {
        m_input_gates[i]->set_bytes_in_counter(metrics->get_IO_metric_group()->get_bytes_in_counter());
    }

    // TODO: setup task's metrics
}

/**
 * The core work method that bootstraps the task and executes its code
 */
void Task::do_run() {
    // -----------------------------------------
    //   setup result partitions and input gates
    // -----------------------------------------
    SPDLOG_LOGGER_INFO(m_logger, "Task {} start running", m_task_name_with_subtask);

    // -----------------------------------------
    //   initialize Environment
    // -----------------------------------------
    std::shared_ptr<Environment> env = std::make_shared<RuntimeEnvironment>(m_job_id, m_vertex_id, m_execution_id, m_result_partitions, m_number_of_result_partitions,
                                                                                m_input_gates, m_number_of_input_gates, m_task_configuration, m_task_info, m_metrics);

    // -----------------------------------------
    //   load Invokable (StreamTask)
    // -----------------------------------------
    this->m_invokable = load_and_instantiate_invokable(m_name_of_invokable_class, env);
    SPDLOG_LOGGER_DEBUG(m_logger, "Task {} loaded invokable {}", m_task_name_with_subtask, m_name_of_invokable_class);

    // -----------------------------------------
    //   actual task core work
    // -----------------------------------------

    this->m_invokable->invoke();

    // -----------------------------------------
    //   finialize of a successful execution
    // -----------------------------------------
    SPDLOG_LOGGER_INFO(m_logger, "Finish task {}", m_task_name_with_subtask);
}


void Task::start_task_thread() {
    SPDLOG_LOGGER_INFO(m_logger, "Task {} begins to work", m_task_name_with_subtask);
    m_executing_thread = std::make_shared<std::thread>(&Task::run, this);

}

void Task::cancel_task() {
    SPDLOG_LOGGER_INFO(m_logger, "Task {} cancel task", m_task_name_with_subtask);
    m_invokable->cancel();

    m_executing_thread->join();
}

// TODO: use a <stream operator identifier, creator> map to create StreamTask
std::shared_ptr<AbstractInvokable> Task::load_and_instantiate_invokable(std::string class_name, std::shared_ptr<Environment> env){
    return StreamTaskFactory<AbstractInvokable>::instance()->create_task(class_name, env);
}