/**
 * Interface for stream operators with one input.
 */

#pragma once

#include "StreamOperator.hpp"
#include "StreamRecordV2.hpp"
#include <memory>
#include <iostream>


template <class IN, class OUT>
class OneInputStreamOperator : virtual public StreamOperator<OUT>
{
private:
public:
    virtual void process_element(StreamRecordV2<IN>* stream_record) {}
};
