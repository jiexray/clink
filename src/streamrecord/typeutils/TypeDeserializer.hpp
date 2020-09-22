/**
 * Deserializer of a buffer. The deserializer is binded to an input channel. No thread safe guarantee.
 */

#pragma once
#include "../../buffer/BufferBase.hpp"
#include "../types/IOReadableWritable.hpp"
#include "../StreamRecord.hpp"
#include "SerializeUtils.hpp"
#include "Constant.hpp"
#include "LoggerFactory.hpp"
#include <iostream>
#include <memory>
#include <deque>

class IOReadableWritable;

class TypeDeserializer
{             
public:
    virtual void                                set_next_buffer(std::shared_ptr<BufferBase> buffer) = 0;
    virtual DeserializationResult               get_next_record(std::shared_ptr<IOReadableWritable> target) = 0;

    virtual int                                 read_short() = 0;
    virtual int                                 read_int() = 0;
    virtual double                              read_double() = 0;
    virtual int                                 read_byte() = 0;
    virtual int                                 read_unsigned_byte() = 0;

    /* Properties */
    virtual int                                 get_record_size() = 0;
    virtual void                                set_record_size(int) = 0;
    virtual std::string                         dump_state() = 0;
};

