#pragma once
#include "StreamRecordV2.hpp"

/**
  Stores the value and the timestamp of the record.

  @param <T> The type encapsulated value
 */
template <class T>
class TimestampedValue
{
private:
    T& m_value;
    long m_timestamp;
    bool m_has_timestamp;

public:
    TimestampedValue(T& value): m_value(value), m_timestamp(0), m_has_timestamp(false) {}

    TimestampedValue(T& value, long timestamp): m_value(value), m_timestamp(timestamp), m_has_timestamp(true) {}

    TimestampedValue(const TimestampedValue& other) {
        m_timestamp = other.m_timestamp;
        m_value = other.m_value;
        m_has_timestamp = other.m_has_timestamp;
    }

    TimestampedValue& operator=(const TimestampedValue& other){
        if (this != &other) {
            this->m_value = other.m_value;
            this->m_timestamp = other.m_timestamp;
            this->m_has_timestamp = other.m_has_timestamp;
        }
        return *this;
    }

    TimestampedValue& operator=(TimestampedValue&& other){
        if (this != &other) {
            this->m_value = other.m_value;
            this->m_timestamp = other.m_timestamp;
            this->m_has_timestamp = other.m_has_timestamp;
        }
        return *this;
    }

    T& get_value() {
        return m_value;
    }

    long get_timestamp() {
        return m_timestamp;
    }

    bool has_timestamp() {
        return m_has_timestamp;
    }

    StreamRecordV2<T>* get_stream_record() {
        StreamRecordV2<T> stream_record = new StreamRecordV2<T>(m_value, m_timestamp);
        return stream_record;
    }

    static TimestampedValue<T>* from(StreamRecordV2<T>& stream_record) {
        if (stream_record.timestamp != -1) {
            return new TimestampedValue<T>(stream_record.val, stream_record.timestamp);
        } else {
            return new TimestampedValue<T>(stream_record.val);
        }
    }
};

