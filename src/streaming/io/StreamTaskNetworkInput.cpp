#include "StreamTaskNetworkInput.hpp"

template <class T>
StreamTaskNetworkInput<T>::StreamTaskNetworkInput(std::shared_ptr<InputGate> input_gate): StreamTaskNetworkInput(input_gate, 0) {}

template <class T>
StreamTaskNetworkInput<T>::StreamTaskNetworkInput(std::shared_ptr<InputGate> input_gate, int input_idx) {
    this->m_input_gate = input_gate;
    this->m_input_idx = input_idx;
    m_deserialization_delegate = std::make_shared<DeserializationDelegate<T>>();

    int num_input_channels = input_gate->get_number_of_input_channels();

    for (int i = 0; i < num_input_channels; i++) {
        m_record_deserializers.push_back(std::make_shared<StreamRecordDeserializer>());
    }
}

template <class T>
InputStatus StreamTaskNetworkInput<T>::emit_next(std::shared_ptr<DataOutput<T>> output) {
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
                std::shared_ptr<StreamRecord<T>> new_record = std::make_shared<StreamRecord<T>>(this->m_deserialization_delegate->get_instance());
                output->emit_record(new_record);

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

template class StreamTaskNetworkInput<std::string>;
template class StreamTaskNetworkInput<double>;
template class StreamTaskNetworkInput<int>;