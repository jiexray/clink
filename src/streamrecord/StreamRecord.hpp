/** 
 * Base class of all types of StreamRecord.
 */

#pragma once
#include <memory>
#include <typeinfo>
#include "Tuple.hpp"

enum StreamRecordAppendResult {
    FULL_RECORD,
    FULL_RECORD_BUFFER_FULL,
    PARTITAL_RECORD_BUFFER_FULL,
    NONE_RECORD // the data length is not correct written
};

enum DeserializationResult {
    PARTIAL_RECORD,
    INTERMEDIATE_RECORD_FROM_BUFFER,
    LAST_RECORD_FROM_BUFFER
};


template<class T>
class StreamRecord
{
private:
    long                                    m_timestamp;
protected:
    std::shared_ptr<void>                      m_value;
public:
    StreamRecord(long timestamp):m_timestamp(timestamp){}
    // StreamRecord(std::shared_ptr<T> value, long timestamp): m_value(value), m_timestamp(timestamp) {}
    // StreamRecord(std::shared_ptr<T> value): m_value(value), m_timestamp(0) {}

    StreamRecord(std::shared_ptr<void> value): StreamRecord(value, 0) {}
    StreamRecord(std::shared_ptr<void> value, long timestamp): m_value(value), m_timestamp(timestamp) {
        // do type check
        if (std::static_pointer_cast<T>(value) == nullptr) {
            throw std::invalid_argument("Can not create a StreamRecord for type " + std::string(__PRETTY_FUNCTION__));
        }
    }

    std::shared_ptr<T>                      get_value() {
        return std::static_pointer_cast<T>(m_value);
    }
    // virtual int                             get_value_size() = 0;
    
    /* TODO: may there is a better solution, I do not need to new a StreamRecord instance, just reuse the old instance. */
    template <class X>
    std::shared_ptr<StreamRecord<X>> replace(std::shared_ptr<X> value) {
        return std::make_shared<StreamRecord<X>>(value, this->m_timestamp);
    }
    long                                    get_timestamp() {return m_timestamp;}
    long                                    set_timestamp(long timestamp) {m_timestamp = timestamp;}
    
    // virtual StreamRecordAppendResult        serialize_record_to_buffer_builder(std::shared_ptr<BufferBuilder> buffer_builder) {};
};
