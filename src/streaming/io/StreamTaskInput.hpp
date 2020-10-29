/**
 * Basic interface for inputs of stream operators. It is located in StreamInputProcessor, which is the 
 * base module for input processing in a StreamTask. It is one component of (StreamTaskInput, DataOutput)-pair.
 */
#pragma once
#include "InputStatus.hpp"
#include "DataOutput.hpp"
#include <memory>
#include "AvailabilityProvider.hpp"

template <class T>
class StreamTaskInput: public AvailabilityProvider
{
private:
public:
    virtual InputStatus     emit_next(std::shared_ptr<DataOutput<T>> output) = 0;
    virtual int             get_input_index() = 0;
    virtual void            close() {}
};
