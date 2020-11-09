/**
 * Basic data output interface used in emitting the next element from data input. It is located in StreamInputProcessor, which is the 
 * base module for input processing in a StreamTask. It is one component of (StreamTaskInput, DataOutput)-pair.
 */
#pragma once
// #include "StreamRecord.hpp"
#include "StreamRecordV2.hpp"
#include "Counter.hpp"

template <class T>
class DataOutput
{
private:
public:
    virtual void emit_record(StreamRecordV2<T>* stream_record) = 0;
    virtual void close() {}
    virtual void set_num_records_in(std::shared_ptr<Counter>) {}
};
