/**
 * The OperatorChain contains all operators that are executed as one chain within a single StreamTask.
 */
#pragma once 
#include <memory>
#include "ResultWriterOutput.hpp"
#include "StreamOperatorFactory.hpp"
// template <class OUT> class StreamOperatorFactory;

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
    OperatorChain(std::shared_ptr<ResultWriter<OUT>> result_writer, 
            std::shared_ptr<StreamOperatorFactory<OUT>> operator_factory);

    std::shared_ptr<ResultWriterOutput<OUT>>    create_result_stream_output();

    void                                        flush_output() {this->m_stream_output->flush();}


    /* Properties */
    std::shared_ptr<StreamOperator<OUT>>        get_head_operator() {return m_core_operator;}
};


template <class OUT>
inline OperatorChain<OUT>::OperatorChain(std::shared_ptr<ResultWriter<OUT>> result_writer, std::shared_ptr<StreamOperatorFactory<OUT>> operator_factory):
m_result_writer(result_writer) {
    this->m_stream_output = create_result_stream_output();
    // this->m_core_operator = operator_factory->create_stream_operator(std::make_shared<StreamOperatorParameters<OUT>>(containing_task, this->m_stream_output));
    this->m_core_operator = operator_factory->create_stream_operator(std::make_shared<StreamOperatorParameters<OUT>>(this->m_stream_output));
}

template <class OUT>
inline std::shared_ptr<ResultWriterOutput<OUT>> OperatorChain<OUT>::create_result_stream_output() {
    return std::make_shared<ResultWriterOutput<OUT>>(this->m_result_writer);
}
