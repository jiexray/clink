/**
 * Implementation of StreamTaskInput that wraps an input from network taken from InputGate.
 */
#pragma once

#include "../../result/consumer/InputGate.hpp"
#include "StreamTaskInput.hpp"
#include "../../streamrecord/StreamRecord.hpp"
#include "../../result/consumer/BufferOrEvent.hpp"
#include "../../streamrecord/StreamRecordDeserializer.hpp"
#include "../../streamrecord/types/DeserializationDelegate.hpp"
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
    std::shared_ptr<DeserializationDelegate<T>>             m_deserialization_delegate;

    /* deserialization array of all input channels */
    std::shared_ptr<StreamRecordDeserializer>               m_current_record_deserializer;
    std::vector<std::shared_ptr<StreamRecordDeserializer>>  m_record_deserializers; // one input channel has an exclusive deserializer

public:
    StreamTaskNetworkInput(std::shared_ptr<InputGate> input_gate);
    StreamTaskNetworkInput(std::shared_ptr<InputGate> input_gate, int input_idx);

    /* Properties */
    int                                                     get_input_index() {return m_input_idx;}

    /* emit input data record */
    InputStatus                                             emit_next(std::shared_ptr<DataOutput<T>> output);
};

