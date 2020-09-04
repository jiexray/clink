/**
 * Basic data output interface used in emitting the next element from data input. It is located in StreamInputProcessor, which is the 
 * base module for input processing in a StreamTask. It is one component of (StreamTaskInput, DataOutput)-pair.
 */
#pragma once
#include "../../streamrecord/StreamRecord.hpp"

template <class T>
class DataOutput
{
private:
public:
    virtual void emit_record(std::shared_ptr<StreamRecord<T>> stream_record) = 0;
};
