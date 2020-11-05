#include "TupleDeserializerV2.hpp"
std::shared_ptr<spdlog::logger> TupleDeserializerV2::m_logger = LoggerFactory::get_logger("TupleDeserializer");

DeserializationResult TupleDeserializerV2::get_next_record(std::shared_ptr<IOReadableWritable> target) {
    // SPDLOG_LOGGER_INFO(m_logger, "TupleDeserialize state: {}", dump_state());
    // first check if can read length of tuple, if not, return PARTIAL_RECORD, directly
    if (m_remaining < 2) {
        // SPDLOG_LOGGER_ERROR(m_logger, "Insufficient length to read a short! remaining: {}", m_type_deserializer->get_remaining());
        return DeserializationResult::PARTIAL_RECORD;   
    }

    if (m_record_fields == -1) {
        // a new read
        m_record_fields = 2;

        m_record_size = read_short();
    }

    if (m_record_size > m_remaining) {
        // The full tuple is not all received
        return DeserializationResult::PARTIAL_RECORD;
    }
    
    target->read(shared_from_this(), m_record_size);

    // finish one tuple, reset m_record_field to fresh
    m_record_fields = -1;

    // Note: There may be are some data left to read in TypeDeserialize, check and reread
    return m_remaining == 0 ? DeserializationResult::LAST_RECORD_FROM_BUFFER : DeserializationResult::INTERMEDIATE_RECORD_FROM_BUFFER;
}