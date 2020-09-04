/**
 * Base class for all streaming tasks. A task is the unit of local processing that is deployed 
 * and executed by the TaskManagers.
 * 
 * Note: current one StreamTask only have one ResultWriter, which write writes data to subtasks
 *  in the downstream task.
 */
#pragma once
#include "OperatorChain.hpp"
#include "../io/StreamInputProcessor.hpp"
#include "../io/StreamTaskInput.hpp"
#include "../io/DataOutput.hpp"

#include "../../result/ResultWriter.hpp"
#include "../../result/ChannelSelectorResultWriter.hpp"
#include "../../runtime/AbstractInvokable.hpp"
#include "../graph/StreamEdge.hpp"
#include "config/StreamConfig.hpp"
#include <memory>

template <class OUT> class OperatorChain;
template <class OUT> class StreamEdge;
class StreamConfig;

template <class OUT>
class StreamTask : public AbstractInvokable, public std::enable_shared_from_this<StreamTask<OUT>>
{
private:
    std::shared_ptr<ResultWriter<OUT>>          m_result_writer;
protected:
    std::shared_ptr<StreamInputProcessor>       m_input_processor;
    std::shared_ptr<StreamOperator<OUT>>        m_head_operator;
    std::shared_ptr<OperatorChain<OUT>>         m_operator_chain;
    std::shared_ptr<StreamConfig>               m_configuration;
public:
    // for test
    StreamTask() {};
    StreamTask(std::shared_ptr<Environment> env);

    /* Life cycle methods */
    virtual void                                init() {};
    void                                        process_input();

    /* Core work methods */
    void                                        before_invoke();
    void                                        request_partitions();
    void                                        invoke() override;

    /* Util functions */
    std::shared_ptr<ResultWriter<OUT>>          create_result_writer(std::shared_ptr<StreamEdge<OUT>> edge, int output_idx, std::string task_name);

    /* Properties */
    std::shared_ptr<StreamConfig>               get_configuration() {return m_configuration;}
    std::shared_ptr<StreamOperator<OUT>>        get_head_operator() {return m_head_operator;}
    std::shared_ptr<ResultWriter<OUT>>          get_result_writer() {return m_result_writer;}
};

