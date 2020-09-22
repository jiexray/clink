#include "TupleDeserializer.hpp"

DeserializationResult TupleDeserializer::get_next_record(std::shared_ptr<IOReadableWritable> target) {
    if (m_record_fields == -1) {
        // a new read
        m_record_fields = std::dynamic_pointer_cast<TupleDeserializationDelegate>(target)->get_num_of_values();

        m_record_size = read_short();
    }

    if (m_record_size > m_type_deserializer->get_remaining()) {
        // The full tuple is not all received
        return DeserializationResult::PARTIAL_RECORD;
    }
    
    DeserializationResult partial_result;
    for (int i = 0; i < m_record_fields; i++) {
        std::shared_ptr<IOReadableWritable> partial_target = target->get_field(i);
        if (partial_target == nullptr) {
            throw std::runtime_error("Field in tuple DeserializationDelegate is not set properly");
        }
        // do a redundant read to get the length of the data
        int partial_record_size = read_short();
        // set the record size in inner type deserialzer, the Value will use this value to read buf
        m_type_deserializer->set_record_size(partial_record_size);

        partial_result = m_type_deserializer->read_into(partial_target);
    }
    return partial_result;
}
