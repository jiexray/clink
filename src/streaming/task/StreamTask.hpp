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


template <class OUT> class OperatorChain;
template <class OUT> class StreamEdge;
class StreamConfig;


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
    static std::shared_ptr<spdlog::logger>      m_logger;
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
    std::shared_ptr<StreamTask<OUT>> m_stream_task;
public:
    StreamTaskDefaultMailboxAction(std::shared_ptr<StreamTask<OUT>> stream_task): m_stream_task(stream_task){}

    void run_default_action() {
        // std::cout << "StreamTask process_input()" << std::endl;
        m_stream_task->process_input();
    }
};
