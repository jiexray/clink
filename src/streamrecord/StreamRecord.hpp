/** 
 * Base class of all types of StreamRecord.
 */

#pragma once
#include <memory>

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
    std::shared_ptr<T>                      m_value;
public:
    StreamRecord(long timestamp):m_timestamp(timestamp){}
    StreamRecord(std::shared_ptr<T> value, long timestamp): m_value(value), m_timestamp(timestamp) {}
    StreamRecord(std::shared_ptr<T> value): m_value(value), m_timestamp(0) {}
    StreamRecord(T value): m_timestamp(0) {this->m_value = std::make_shared<T>(value);}

    std::shared_ptr<T>                      get_value() {return m_value;}
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
