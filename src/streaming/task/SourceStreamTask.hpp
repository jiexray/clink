/**
 * StreamTask for executing a StreamSource.
 */
#pragma once
#include "StreamTask.hpp"
#include <thread>
#include "OperatorMetricGroup.hpp"


template <class OUT>
class SourceStreamTask : public StreamTask<OUT>
{
private:
    typedef std::shared_ptr<OperatorMetricGroup>    OperatorMetricGroupPtr;
    std::shared_ptr<std::thread>    source_thread;
public:
    SourceStreamTask(std::shared_ptr<Environment> env): StreamTask<OUT>(env){}


    void                            init() override;

    void                            process_input();

    void                            source_thread_run();
};


template <class OUT>
inline void SourceStreamTask<OUT>::init() {
    OperatorMetricGroupPtr operator_metric_group = this->m_task_metric_group->get_or_add_operator(this->m_configuration->get_operator_id(), this->m_configuration->get_operator_name());
    std::shared_ptr<StreamOperatorFactory<OUT>> operator_factory = this->m_configuration->template get_stream_operator_factory<std::string, OUT>();

    this->m_operator_chain = std::make_shared<OperatorChain<OUT>>(this->get_result_writer(), operator_factory, operator_metric_group);

    if (this->m_operator_chain == nullptr) {
        throw std::runtime_error("Fail to create OperatorChain when initializing SourceStreamTask");
    }
    this->m_head_operator = this->m_operator_chain->get_head_operator();
}

template <class OUT>
inline void SourceStreamTask<OUT>::process_input() {
    SPDLOG_LOGGER_DEBUG(this->m_logger, "Task {} start to process streaming input", this->get_name());
    source_thread = std::make_shared<std::thread>(&SourceStreamTask::source_thread_run, this);

    source_thread->join();

    this->cancel();
}

template <class OUT>
inline void SourceStreamTask<OUT>::source_thread_run() {
    (std::dynamic_pointer_cast<StreamSource<OUT>>(this->m_head_operator))->run();
}