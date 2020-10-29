/**
 * Implementation of StreamTaskInput that wraps an input from network taken from InputGate.
 */
#pragma once

#include "InputGate.hpp"
#include "StreamTaskInput.hpp"
#include "StreamRecord.hpp"
#include "BufferOrEvent.hpp"
#include "StreamRecordDeserializer.hpp"
#include "DeserializationDelegate.hpp"
#include "TupleDeserializationDelegate.hpp"
#include "Tuple.hpp"
#include "LoggerFactory.hpp"
#include <memory>
#include <vector>

template <class T>
class StreamTaskNetworkInput : public StreamTaskInput<T>
{
private:
    std::shared_ptr<InputGate>                              m_input_gate;
    int                                                     m_last_channel; // the last input channel, which return by poll_next() of InputGate
    int                                                     m_input_idx; // identify the index of an input gate if there are multiple input gates
                                                                         // locate in a single StreamTask.
                                                                         // Currently, only one input gate supported
    std::shared_ptr<StreamRecord<T>>                        m_current_record;
    std::shared_ptr<IOReadableWritable>                     m_deserialization_delegate;

    /* deserialization array of all input channels */
    std::shared_ptr<StreamRecordDeserializer>               m_current_record_deserializer;
    std::vector<std::shared_ptr<StreamRecordDeserializer>>  m_record_deserializers; // one input channel has an exclusive deserializer


    RECORD_TYPE                                             m_record_type;

    static std::shared_ptr<spdlog::logger>                  m_logger;

public:
    StreamTaskNetworkInput(std::shared_ptr<InputGate> input_gate): StreamTaskNetworkInput(input_gate, 0) {}
    StreamTaskNetworkInput(std::shared_ptr<InputGate> input_gate, int input_idx){
        this->m_input_gate = input_gate;
        this->m_input_idx = input_idx;
        this->m_record_type = RECORD_TYPE::NORMAL;

        m_deserialization_delegate = std::make_shared<DeserializationDelegate<T>>();

        int num_input_channels = input_gate->get_number_of_input_channels();

        for (int i = 0; i < num_input_channels; i++) {
            m_record_deserializers.push_back(std::make_shared<StreamRecordDeserializer>());
        }
    }
    StreamTaskNetworkInput(std::shared_ptr<InputGate> input_gate, int input_idx, std::shared_ptr<IOReadableWritable> deserialization_delegate, RECORD_TYPE record_type) {
        this->m_input_gate = input_gate;
        this->m_input_idx = input_idx;
        this->m_record_type = RECORD_TYPE::TUPLE;

        m_deserialization_delegate = deserialization_delegate;
        

        int num_input_channels = input_gate->get_number_of_input_channels();

        for (int i = 0; i < num_input_channels; i++) {
            m_record_deserializers.push_back(std::make_shared<StreamRecordDeserializer>(RECORD_TYPE::TUPLE));
        }
    }

    void                                                    close() {
        // TODO: close the input
    }

    /* Properties */
    int                                                     get_input_index() {return m_input_idx;}

    /* emit input data record */
    InputStatus                                             emit_next(std::shared_ptr<DataOutput<T>> output){
        while (true)
        {
            if (this->m_current_record_deserializer != nullptr) {
                // use a delegate to get the data from deserializer
                DeserializationResult result = m_current_record_deserializer->get_next_record(this->m_deserialization_delegate);

                if (result == DeserializationResult::PARTIAL_RECORD || result == DeserializationResult::LAST_RECORD_FROM_BUFFER) {
                    // a failure deserialization, m_current_record_deserializer is not available currently
                    m_current_record_deserializer = nullptr;
                }

                if (result == DeserializationResult::INTERMEDIATE_RECORD_FROM_BUFFER || result == DeserializationResult::LAST_RECORD_FROM_BUFFER) {
                    // create a StreamRecord based on the deserialization delegate.
                    if (m_record_type == RECORD_TYPE::NORMAL) {
                        std::shared_ptr<StreamRecord<T>> new_record = std::make_shared<StreamRecord<T>>(
                                                                            std::dynamic_pointer_cast<DeserializationDelegate<T>>(m_deserialization_delegate)->get_instance());
                        output->emit_record(new_record);
                    } else if (m_record_type == RECORD_TYPE::TUPLE) {
                        std::shared_ptr<StreamRecord<T>> new_record = std::make_shared<StreamRecord<T>>(
                                                                            std::dynamic_pointer_cast<TupleDeserializationDelegate>(m_deserialization_delegate)->get_instance());
                        output->emit_record(new_record);
                    }
            
                    
                    // deserializer conduct a successful record deserialization, we have delivered & processed it to the operator through DataOutput.
                    // There may be more available records in the deserializer's buffer, however, we temporialy return and let the StreamTask
                    // know one record have been processed.
                    return InputStatus::MORE_AVAILABLE;
                }
            }
            std::shared_ptr<BufferOrEvent> boe = this->m_input_gate->poll_next();

            if (boe != nullptr) {
                m_last_channel = boe->get_channel_idx();
                m_current_record_deserializer = m_record_deserializers[m_last_channel];

                if (m_current_record_deserializer == nullptr) {
                    throw std::runtime_error("record deserializer of " + std::to_string(m_last_channel) + " is null");
                }
                m_current_record_deserializer->set_next_buffer(boe->get_buffer());

                // buffer may be already for deserializing, continue to deserialize the buffer
            } else {
                return InputStatus::NOTHING_AVAILABLE;
            }
        }
    }

    AvailabilityProvider::CompletableFuturePtr              get_available_future() override {
        if (m_current_record_deserializer != nullptr) {
            return AvailabilityProvider::AVAILABLE;
        }
        return m_input_gate->get_available_future();
    }
};

template<class OUT>
std::shared_ptr<spdlog::logger> StreamTaskNetworkInput<OUT>::m_logger = LoggerFactory::get_logger("StreamTaskNetworkInput");