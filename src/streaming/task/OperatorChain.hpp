/**
 * The OperatorChain contains all operators that are executed as one chain within a single StreamTask.
 */
#pragma once 
#include <memory>
// #include "../../result/ResultWriter.hpp"
#include "../io/ResultWriterOutput.hpp"
#include "../operators/StreamOperatorFactory.hpp"
#include "StreamTask.hpp"

template <class OUT> class StreamTask;
template <class OUT> class StreamOperatorFactory;

template <class OUT>
class OperatorChain
{
private:
    // Write record to result partition through result writer.
    std::shared_ptr<ResultWriter<OUT>>          m_result_writer;

    // the core operator for this operator chain.
    // TODO: implement real operator chain, which connect each other through a link between consequtive operators.
    std::shared_ptr<StreamOperator<OUT>>        m_core_operator;

    // only one ResultWriter for each OperatorChain
    std::shared_ptr<ResultWriterOutput<OUT>>    m_stream_output;

public:
    OperatorChain(std::shared_ptr<StreamTask<OUT>> containing_task, 
            std::shared_ptr<ResultWriter<OUT>> result_writer, 
            std::shared_ptr<StreamOperatorFactory<OUT>> operator_factory);

    std::shared_ptr<ResultWriterOutput<OUT>>    create_result_stream_output();

    void                                        flush_output() {this->m_stream_output->flush();}


    /* Properties */
    std::shared_ptr<StreamOperator<OUT>>        get_head_operator() {return m_core_operator;}
};

