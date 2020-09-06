/**
 * StreamTask for executing a StreamSource.
 */
#pragma once
#include "StreamTask.hpp"
#include <thread>


template <class OUT>
class SourceStreamTask : public StreamTask<OUT>
{
private:
    std::shared_ptr<std::thread>    source_thread;
public:
    SourceStreamTask(std::shared_ptr<Environment> env): StreamTask<OUT>(env){
    }

    void                            init() override;

    void                            process_input();

    void                            source_thread_run();
};


template <class OUT>
void SourceStreamTask<OUT>::init() {
    // The first template parameter is no-use, just a placeholder
    std::shared_ptr<StreamOperatorFactory<OUT>> operator_factory = this->m_configuration->template get_stream_operator_factory<std::string, OUT>();

    this->m_operator_chain = std::make_shared<OperatorChain<OUT>>(this->shared_from_this(), 
                                                                this->get_result_writer(), 
                                                                operator_factory);
    if (this->m_operator_chain == nullptr) {
        throw std::runtime_error("Fail to create OperatorChain when initializing SourceStreamTask");
    }
    this->m_head_operator = this->m_operator_chain->get_head_operator();
}

template <class OUT>
void SourceStreamTask<OUT>::process_input() {
    std::cout  << "SourceStreamTask<OUT>::process_input()" << std::endl;
    source_thread = std::make_shared<std::thread>(&SourceStreamTask::source_thread_run, this);

    source_thread->join();
}

template <class OUT>
void SourceStreamTask<OUT>::source_thread_run() {
    (std::dynamic_pointer_cast<StreamSource<OUT>>(this->m_head_operator))->run();
}