/**
 * Base class for all streaming tasks. A task is the unit of local processing that is deployed 
 * and executed by the TaskManagers.
 * 
 * Note: current one StreamTask only have one ResultWriter, which write writes data to subtasks
 *  in the downstream task.
 */
#pragma once
#include "OperatorChain.hpp"
#include "StreamInputProcessor.hpp"
#include "StreamTaskInput.hpp"
#include "DataOutput.hpp"

#include "ResultWriter.hpp"
#include "ChannelSelectorResultWriter.hpp"
#include "AbstractInvokable.hpp"
#include "StreamEdge.hpp"
#include "StreamConfig.hpp"
#include "MailboxProcessor.hpp"
#include "Constant.hpp"
#include "LoggerFactory.hpp"
#include <memory>
#include <unistd.h>




template <class OUT>
class StreamTask : public AbstractInvokable, public std::enable_shared_from_this<StreamTask<OUT>>
{
private:
    std::shared_ptr<ResultWriter<OUT>>          m_result_writer;
    int volatile                                m_is_running;
protected:
    std::shared_ptr<StreamInputProcessor>       m_input_processor;
    std::shared_ptr<StreamOperator<OUT>>        m_head_operator;
    std::shared_ptr<OperatorChain<OUT>>         m_operator_chain;
    std::shared_ptr<StreamConfig>               m_configuration;
    std::shared_ptr<MailboxProcessor>           m_mailbox_processor;
    std::shared_ptr<spdlog::logger>             m_logger;

public:
    // for test
    StreamTask() {};
    StreamTask(std::shared_ptr<Environment> env);

    /* Life cycle methods */
    virtual void                                init() {};
    virtual void                                process_input();

    /* Core work methods */
    void                                        before_invoke();
    void                                        request_partitions();
    void                                        invoke() override;
    void                                        cancel() override;

    /* Util functions */
    std::shared_ptr<ResultWriter<OUT>>          create_result_writer(std::shared_ptr<StreamEdge<OUT>> edge, int output_idx, std::string task_name);

    /* Properties */
    std::shared_ptr<StreamConfig>               get_configuration() {return m_configuration;}
    std::shared_ptr<StreamOperator<OUT>>        get_head_operator() {return m_head_operator;}
    std::shared_ptr<ResultWriter<OUT>>          get_result_writer() {return m_result_writer;}
    std::string                                 get_name() {return this->get_environment()->get_task_info()->get_task_name_with_sub_tasks();}
}; 

template <class OUT>
class StreamTaskDefaultMailboxAction: public MailboxDefaultAction {
private:
    std::sh// class StreamConfig;
askDefaultMailboxAction(std::shared_ptr<StreamTask<OUT>> stream_task): m_stream_task(stream_task){}

    void run_default_action() {
        // std::cout << "StreamTask process_input()" << std::endl;
        m_stream_task->process_input();
    }
};

/**
 * Constructor of StreamTask, initialize result writer.
 */
template <class OUT>
inline StreamTask<OUT>::StreamTask(std::shared_ptr<Environment> env): AbstractInvokable(env) {
    spdlog::set_pattern(Constant::SPDLOG_PATTERN);
    spdlog::set_level(Constant::SPDLOG_LEVEL);

    m_is_running = true;
    m_configuration = std::make_shared<StreamConfig>(env->get_task_configuration());
    m_logger = LoggerFactory::get_logger("StreamTask");

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
inline std::shared_ptr<ResultWriter<OUT>> StreamTask<OUT>::create_result_writer(std::shared_ptr<StreamEdge<OUT>> edge, int output_idx, std::string task_name) {
    std::shared_ptr<StreamPartitioner<OUT>> output_partitioner = edge->get_output_partitioner();

    SPDLOG_LOGGER_INFO(m_logger, "Using partitioner {} for output {} of StreamTask {}", output_partitioner->to_string(), output_idx, task_name);

    std::shared_ptr<ResultPartition> buffer_writter = this->get_environment()->get_writer(output_idx);
    return std::make_shared<ChannelSelectorResultWriter<OUT>>(buffer_writter, task_name, output_partitioner);
}

template <class OUT>
inline void StreamTask<OUT>::process_input() {
    InputStatus status = m_input_processor->process_input();
    // TODO: process the return status of input_procesor
}

template<class OUT>
inline void StreamTask<OUT>::before_invoke() {
    this->m_mailbox_processor = std::make_shared<MailboxProcessor>(std::make_shared<StreamTaskDefaultMailboxAction<OUT>>(this->shared_from_this()), 
                                                                    std::make_shared<TaskMailbox>(nullptr));

    init();

    // request partitions
    request_partitions();
}

template<class OUT>
inline void StreamTask<OUT>::request_partitions() {
    std::shared_ptr<InputGate> input_gate = get_environment()->get_input_gate(0);
    if (input_gate != nullptr) {
        input_gate->request_partitions();
    }
}

template<class OUT>
inline void StreamTask<OUT>::invoke() {
    before_invoke();

    SPDLOG_LOGGER_INFO(m_logger, "StreamTask {} start to process records", get_name());
    m_mailbox_processor->run_mailbox_loop();
}

template <class OUT>
inline void StreamTask<OUT>::cancel() {
    m_is_running = false;
    SPDLOG_LOGGER_DEBUG(m_logger, "Stop StreamTask {}", get_name());
    if (m_mailbox_processor == nullptr) {
        throw std::runtime_error("mailbox processor is null");
    }
    m_mailbox_processor->all_actions_completed();
}

