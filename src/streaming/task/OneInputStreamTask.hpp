/**
 * A Streamtask for executing a OneInputStreamOperator.
 */

#pragma once

#include "StreamTask.hpp"
#include "../io/StreamTaskNetworkInput.hpp"
#include "../io/StreamTaskNetworkOutput.hpp"
#include "../io/StreamOneInputProcessor.hpp"
#include "StreamTaskNetworkInputFactory.hpp"
#include "TemplateHelper.hpp"

template <class IN, class OUT = NullType> class OneInputStreamTask;

template <class IN, class OUT>
class OneInputStreamTask: public StreamTask<OUT> 
{
public:
    // TODO: initialize StreamTask
    OneInputStreamTask(std::shared_ptr<Environment> env): StreamTask<OUT>(env) {}

    void                        init() {
        std::shared_ptr<StreamOperatorFactory<OUT>> operator_factory = this->m_configuration->template get_stream_operator_factory<IN, OUT, TemplateHelper<OUT>::is_null_type>();

        this->m_operator_chain = std::make_shared<OperatorChain<OUT>>(this->get_result_writer(), 
                                                                    operator_factory);
        this->m_head_operator = this->m_operator_chain->get_head_operator();
        

        int number_of_inputs = this->m_configuration->get_number_of_inputs();

        // std::cout << "[DEBUG] OneInputStreamTask<IN, OUT>::init() after get number_of_inputs: " << number_of_inputs << std::endl;
        SPDLOG_LOGGER_DEBUG(this->m_logger, "OneInputStreamTask<IN, OUT>::init() after get number_of_inputs: {}", number_of_inputs);

        if (number_of_inputs > 0) {
            // currently, we only support one InputGate per StreamTask
            std::shared_ptr<InputGate> input_gate = this->get_environment()->get_input_gate(0);

            // Use StreamTaskNetworkInputFactory to create NetworkInput
            std::shared_ptr<StreamTaskInput<IN>> input  = std::make_shared<StreamTaskNetworkInput<IN>>(input_gate);
            std::shared_ptr<DataOutput<IN>> output      = std::make_shared<StreamTaskNetworkOutput<IN, OUT>>(
                                                            std::dynamic_pointer_cast<OneInputStreamOperator<IN, OUT>>(this->get_head_operator()));
            this->m_input_processor                     = std::make_shared<StreamOneInputProcessor<IN>>(input, output);
        }
    }
};


template <template <class, class> class IN, class OUT, class IN1, class IN2>
class OneInputStreamTask<IN<IN1, IN2>, OUT>: public StreamTask<OUT> 
{
public:
    OneInputStreamTask(std::shared_ptr<Environment> env): StreamTask<OUT>(env) {
    }

    void                        init() {
        std::shared_ptr<StreamOperatorFactory<OUT>> operator_factory = this->m_configuration->template get_stream_operator_factory<IN<IN1, IN2>, OUT, TemplateHelper<OUT>::is_null_type>();
        // this->m_operator_chain = std::make_shared<OperatorChain<OUT>>(this->shared_from_this(), 
        //                                                             this->get_result_writer(), 
        //                                                             operator_factory);
        this->m_operator_chain = std::make_shared<OperatorChain<OUT>>(this->get_result_writer(), 
                                                                    operator_factory);
        this->m_head_operator = this->m_operator_chain->get_head_operator();
        

        int number_of_inputs = this->m_configuration->get_number_of_inputs();

        // std::cout << "[DEBUG] OneInputStreamTask<IN, OUT>::init() after get number_of_inputs: " << number_of_inputs << std::endl;
        SPDLOG_LOGGER_DEBUG(this->m_logger, "OneInputStreamTask<IN, OUT>::init() after get number_of_inputs: {}", number_of_inputs);

        if (number_of_inputs > 0) {
            // currently, we only support one InputGate per StreamTask
            std::shared_ptr<InputGate> input_gate = this->get_environment()->get_input_gate(0);

            // Use StreamTaskNetworkInputFactory to create NetworkInput
            std::shared_ptr<StreamTaskInput<IN<IN1, IN2>>> input  = StreamTaskNetworkInputFactory<IN<IN1, IN2>>::create_stream_task_network_input(input_gate);
            std::shared_ptr<DataOutput<IN<IN1, IN2>>> output      = std::make_shared<StreamTaskNetworkOutput<IN<IN1, IN2>, OUT>>(
                                                            std::dynamic_pointer_cast<OneInputStreamOperator<IN<IN1, IN2>, OUT>>(this->get_head_operator()));
            this->m_input_processor                               = std::make_shared<StreamOneInputProcessor<IN<IN1, IN2>>>(input, output);
        }
    }
};