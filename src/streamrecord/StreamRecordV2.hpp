/**
 * StreamRecord Version 2, no need to serialize or deserialize.
 */

#pragma once
#include "TemplateHelper.hpp"
#include <string>
#include <cstring>

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

template <class T, int SIZE> struct StreamRecordV2;

/**
  Record for based type records
 */
template <class T, int SIZE = sizeof(T)>
struct StreamRecordV2 {
    long timestamp;
    T val;

    StreamRecordV2(){}
    StreamRecordV2(const T* v): StreamRecordV2(*v, -1){}
    StreamRecordV2(T v):StreamRecordV2(v, -1){}
    StreamRecordV2(T v, long t): val(v), timestamp(t){}
};

template <>
struct StreamRecordV2<const char[32], 32> {
    long timestamp;
    char val[32];

    StreamRecordV2(){}
    StreamRecordV2(const char** val): StreamRecordV2(*val, -1){}
    StreamRecordV2(const char* str): StreamRecordV2(str, -1){}
    StreamRecordV2(const char* str, long t): timestamp(t) {
        if (strlen(str) >= 32) {
            throw std::runtime_error("Buffer in StreamRecord is insufficient, str: " + std::string(str) + ", len: "
            + std::to_string(strlen(str)) 
            + " >= " + std::to_string(32));
        }
        strncpy(val, str, 32);
    }
};

template <int SIZE>
struct StreamRecordV2<const char*, SIZE> {
    long timestamp;
    char val[SIZE];

    StreamRecordV2(){}
    StreamRecordV2(const char** val): StreamRecordV2(*val, -1){}
    StreamRecordV2(const char* str): StreamRecordV2(str, -1){}
    StreamRecordV2(const char* str, long t): timestamp(t) {
        if (strlen(str) >= SIZE) {
            throw std::runtime_error("Buffer in StreamRecord is insufficient, str: " + std::string(str) + ", len: "
            + std::to_string(strlen(str)) 
            + " >= " + std::to_string(SIZE));
        }
        strncpy(val, str, SIZE);
    }
};