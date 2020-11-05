#include "TupleDeserializer.hpp"

std::shared_ptr<spdlog::logger> TupleDeserializer::m_logger = LoggerFactory::get_logger("TupleDeserializer");

DeserializationResult TupleDeserializer::get_next_record(std::shared_ptr<IOReadableWritable> target) {
    // first check if can read length of tuple, if not, return PARTIAL_RECORD, directly
    if (m_type_deserializer->get_remaining() < 2) {
        // SPDLOG_LOGGER_ERROR(m_logger, "Insufficient length to read a short! remaining: {}", m_type_deserializer->get_remaining());
        return DeserializationResult::PARTIAL_RECORD;   
    }

    if (m_record_fields == -1) {
        // a new read
        // m_record_fields = std::dynamic_pointer_cast<TupleDeserializationDelegate>(target)->get_num_of_values();
        m_record_fields = target->get_num_of_values();

        m_record_size = read_short();
        m_remaining = m_record_size;
    }

    if (m_remaining > m_type_deserializer->get_remaining()) {
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

        // Note: update remaining buffer to read
        m_remaining -= 2;

        partial_result = m_type_deserializer->read_into(partial_target);

        m_remaining -= partial_record_size;
    }
    
    // finish one tuple, reset m_record_field to fresh
    m_record_fields = -1;

    // Note: There may be are some data left to read in TypeDeserialize, check and reread
    return m_type_deserializer->get_remaining() == 0 ? DeserializationResult::LAST_RECORD_FROM_BUFFER : DeserializationResult::INTERMEDIATE_RECORD_FROM_BUFFER;
    // return partial_result;
}
