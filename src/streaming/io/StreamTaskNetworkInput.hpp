/**
 * Implementation of StreamTaskInput that wraps an input from network taken from InputGate.
 */
#pragma once

#include "InputGate.hpp"
#include "StreamTaskInput.hpp"
#include "StreamRecordV2.hpp"
#include "BufferOrEvent.hpp"
#include "StreamRecordV2Deserializer.hpp"
#include "LoggerFactory.hpp"
#include "TemplateHelper.hpp"
#include <memory>
#include <vector>
#include <ctime>

// template <class T>
// class StreamTaskNetworkInput;

template <class T>
class StreamTaskNetworkInput : public StreamTaskInput<T>
{
private:
    std::shared_ptr<InputGate>                              m_input_gate;
    int                                                     m_last_channel; // the last input channel, which return by poll_next() of InputGate
    int                                                     m_input_idx; // identify the index of an input gate if there are multiple input gates
                                                                         // locate in a single StreamTask.
                                                                         // Currently, only one input gate supported
    std::shared_ptr<StreamRecordV2<T>>                      m_current_record;


    /* deserialization array of all input channels */

    std::shared_ptr<StreamRecordV2Deserializer<StreamRecordV2<T>>>          m_current_record_deserializer;
    std::vector<std::shared_ptr<StreamRecordV2Deserializer<StreamRecordV2<T>>>> m_record_deserializers;


    static std::shared_ptr<spdlog::logger>                  m_logger;

public:
    StreamTaskNetworkInput(std::shared_ptr<InputGate> input_gate): StreamTaskNetworkInput(input_gate, 0) {}
    StreamTaskNetworkInput(std::shared_ptr<InputGate> input_gate, int input_idx){
        this->m_input_gate = input_gate;
        this->m_input_idx = input_idx;

        int num_input_channels = input_gate->get_number_of_input_channels();

        for (int i = 0; i < num_input_channels; i++) {
            m_record_deserializers.push_back(std::make_shared<StreamRecordV2Deserializer<StreamRecordV2<T>>>());
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
                DeserializationResult result = this->m_current_record_deserializer->get_next_record();

                if (result == DeserializationResult::INTERMEDIATE_RECORD_FROM_BUFFER || result == DeserializationResult::LAST_RECORD_FROM_BUFFER) {
                    
                    StreamRecordV2<T>* new_record = this->m_current_record_deserializer->get_instance();
                    output->emit_record(new_record);
                    // commit one record read, maybe it is a none-copy read
                    this->m_current_record_deserializer->read_commit();

                    if (result == DeserializationResult::PARTIAL_RECORD || result == DeserializationResult::LAST_RECORD_FROM_BUFFER) {
                        // a failure deserialization, m_current_record_deserializer is not available currently
                        this->m_current_record_deserializer = nullptr;
                    }
                    
                    // deserializer conduct a successful record deserialization, we have delivered & processed it to the operator through DataOutput.
                    // There may be more available records in the deserializer's buffer, however, we temporialy return and let the StreamTask
                    // know one record have been processed.
                    return InputStatus::MORE_AVAILABLE;
                }

                if (result == DeserializationResult::PARTIAL_RECORD || result == DeserializationResult::LAST_RECORD_FROM_BUFFER) {
                    // a failure deserialization, m_current_record_deserializer is not available currently
                    this->m_current_record_deserializer = nullptr;
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