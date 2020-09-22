#include "StreamTask.hpp"

template <class OUT>
std::shared_ptr<spdlog::logger> StreamTask<OUT>::m_logger = LoggerFactory::get_logger("StreamTask");

/**
 * Constructor of StreamTask, initialize result writer.
 */
template <class OUT>
StreamTask<OUT>::StreamTask(std::shared_ptr<Environment> env): AbstractInvokable(env) {
    spdlog::set_pattern(Constant::SPDLOG_PATTERN);
    spdlog::set_level(Constant::SPDLOG_LEVEL);

    m_is_running = true;
    m_configuration = std::make_shared<StreamConfig>(env->get_task_configuration());
    std::shared_ptr<StreamEdge<OUT>> edge = m_configuration->get_out_edge<OUT>();

    if (edge != nullptr) {
        this->m_result_writer = create_result_writer(edge, 0, env->get_task_info()->get_task_name());
    } else {
        SPDLOG_LOGGER_INFO(m_logger, "Task {} do not have out edges", env->get_task_info()->get_task_name());
    }
}

/**
 * Create a ResultWriter for the StreamTask.
 * @parameter edge: the edge (logical edge) for this physical subtask.
 * @parameter output_idx: the output index for this stream task in the TaskManager, which 
 *   takes charge of creating ResultPartitions. Currently, the output_idx can only be zero,
 *   for a StreamTask (JobVertex) only have one out edge (represent a StreamEdge between consequtive
 *   tasks).
 * @paramenter task_name: the name of the subtask.
 */
template <class OUT>
std::shared_ptr<ResultWriter<OUT>> StreamTask<OUT>::create_result_writer(std::shared_ptr<StreamEdge<OUT>> edge, int output_idx, std::string task_name) {
    std::shared_ptr<StreamPartitioner<OUT>> output_partitioner = edge->get_output_partitioner();

    SPDLOG_LOGGER_INFO(m_logger, "Using partitioner {} for output {} of StreamTask {}", output_partitioner->to_string(), output_idx, task_name);

    std::shared_ptr<ResultPartition> buffer_writter = this->get_environment()->get_writer(output_idx);
    return std::make_shared<ChannelSelectorResultWriter<OUT>>(buffer_writter, task_name, output_partitioner);
}

template <class OUT>
void StreamTask<OUT>::process_input() {
    InputStatus status = m_input_processor->process_input();
    // TODO: process the return status of input_procesor
}

template<class OUT>
void StreamTask<OUT>::before_invoke() {
    this->m_mailbox_processor = std::make_shared<MailboxProcessor>(std::make_shared<StreamTaskDefaultMailboxAction<OUT>>(this->shared_from_this()), 
                                                                    std::make_shared<TaskMailbox>(nullptr));

    init();

    // request partitions
    request_partitions();
}

template<class OUT>
void StreamTask<OUT>::request_partitions() {
    std::shared_ptr<InputGate> input_gate = get_environment()->get_input_gate(0);
    if (input_gate != nullptr) {
        input_gate->request_partitions();
    }
}

template<class OUT>
void StreamTask<OUT>::invoke() {
    before_invoke();

    SPDLOG_LOGGER_INFO(m_logger, "StreamTask {} start to process records", get_name());
    m_mailbox_processor->run_mailbox_loop();
}

template <class OUT>
void StreamTask<OUT>::cancel() {
    m_is_running = false;
    SPDLOG_LOGGER_DEBUG(m_logger, "Stop StreamTask {}", get_name());
    if (m_mailbox_processor == nullptr) {
        throw std::runtime_error("mailbox processor is null");
    }
    m_mailbox_processor->all_actions_completed();
}

template class StreamTask<std::string>;
