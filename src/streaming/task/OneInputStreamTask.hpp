/**
 * A Streamtask for executing a OneInputStreamOperator.
 */

#pragma once

#include "StreamTask.hpp"
#include "../io/StreamTaskNetworkInput.hpp"
#include "../io/StreamTaskNetworkOutput.hpp"
#include "../io/StreamOneInputProcessor.hpp"

template <class IN, class OUT = std::string>
class OneInputStreamTask: public StreamTask<OUT> 
{
public:
    OneInputStreamTask(std::shared_ptr<Environment> env):StreamTask<OUT>(env) {}

    void                        init() override;
};

template <class IN, class OUT>
void OneInputStreamTask<IN, OUT>::init() {
    std::shared_ptr<StreamOperatorFactory<OUT>> operator_factory = this->m_configuration->template get_stream_operator_factory<IN, OUT>();

    this->m_operator_chain = std::make_shared<OperatorChain<OUT>>(this->shared_from_this(), 
                                                                this->get_result_writer(), 
                                                                operator_factory);
    this->m_head_operator = this->m_operator_chain->get_head_operator();
    

    int number_of_inputs = this->m_configuration->get_number_of_inputs();

    std::cout << "[DEBUG] OneInputStreamTask<IN, OUT>::init() after get number_of_inputs: " << number_of_inputs << std::endl;

    if (number_of_inputs > 0) {
        // currently, we only support one InputGate per StreamTask
        std::shared_ptr<InputGate> input_gate = this->get_environment()->get_input_gate(0);

        std::shared_ptr<StreamTaskInput<IN>> input  = std::make_shared<StreamTaskNetworkInput<IN>>(input_gate);
        std::shared_ptr<DataOutput<IN>> output      = std::make_shared<StreamTaskNetworkOutput<IN, OUT>>(
                                                        std::dynamic_pointer_cast<OneInputStreamOperator<IN, OUT>>(this->get_head_operator()));
        this->m_input_processor                     = std::make_shared<StreamOneInputProcessor<IN>>(input, output);
    }
}