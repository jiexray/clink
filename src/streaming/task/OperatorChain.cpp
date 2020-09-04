#include "OperatorChain.hpp"

template <class OUT>
OperatorChain<OUT>::OperatorChain(std::shared_ptr<StreamTask<OUT>> containing_task,
    std::shared_ptr<ResultWriter<OUT>> result_writer, std::shared_ptr<StreamOperatorFactory<OUT>> operator_factory):
m_result_writer(result_writer) {
    this->m_stream_output = create_result_stream_output();
    this->m_core_operator = operator_factory->create_stream_operator(std::make_shared<StreamOperatorParameters<OUT>>(containing_task, this->m_stream_output));
}

template <class OUT>
std::shared_ptr<ResultWriterOutput<OUT>> OperatorChain<OUT>::create_result_stream_output() {
    return std::make_shared<ResultWriterOutput<OUT>>(this->m_result_writer);
}

template class OperatorChain<std::string>;
template class OperatorChain<double>;