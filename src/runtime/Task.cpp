#include "Task.hpp"

std::shared_ptr<spdlog::logger>  Task::m_logger = LoggerFactory::get_logger("Task");

Task::Task(std::shared_ptr<JobInformation> job_information, std::shared_ptr<TaskInformation> task_information,
        int execution_id, int allocation_id, int subtask_idx, 
        ResultPartitionDeploymentDescriptorList & result_partition_descriptors,
        InputGateDeploymentDescriptorList & input_gate_descriptors,
        std::shared_ptr<ShuffleEnvironment> shuffle_environment,
        std::shared_ptr<BufferPool> buffer_pool){
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

    /* Initializer ResultPartition and InputGates */
    this->m_number_of_result_partitions     = (int) result_partition_descriptors.size();
    this->m_result_partitions               = shuffle_environment->create_result_partitions(m_task_name_with_subtask, result_partition_descriptors, m_buffer_pool);

    this->m_number_of_input_gates           = (int) input_gate_descriptors.size();
    this->m_input_gates                     = shuffle_environment->create_input_gates(m_task_name_with_subtask, input_gate_descriptors);

    spdlog::set_pattern(Constant::SPDLOG_PATTERN);
    spdlog::set_level(Constant::SPDLOG_LEVEL);
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
                                                                                m_input_gates, m_number_of_input_gates, m_task_configuration, m_task_info);

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

std::shared_ptr<AbstractInvokable> Task::load_and_instantiate_invokable(std::string class_name, std::shared_ptr<Environment> env){
    if (class_name == "OneInputStreamTask<std::string, std::string>") {
        return std::make_shared<OneInputStreamTask<std::string, std::string>>(env);
    } else if (class_name == "SourceStreamTask<std::string>") {
        return std::make_shared<SourceStreamTask<std::string>>(env);
    } else if (class_name == "SourceStreamTask<Tuple2<std::string, int>>") {
        return std::make_shared<SourceStreamTask<Tuple2<std::string, int>>>(env);
    } else if (class_name == "OneInputStreamTask<Tuple2<std::string, int>, std::string>") {
        return std::make_shared<OneInputStreamTask<Tuple2<std::string, int>, std::string>>(env);
    } else {
        throw std::runtime_error("unknown type of invokable task: " + class_name);
    }
}