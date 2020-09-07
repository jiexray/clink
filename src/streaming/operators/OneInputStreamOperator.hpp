/**
 * Interface for stream operators with one input.
 */

#pragma once

#include "StreamOperator.hpp"
#include "../../streamrecord/StreamRecord.hpp"
#include <memory>
#include <iostream>


template <class IN, class OUT>
class OneInputStreamOperator : virtual public StreamOperator<OUT>
//: public StreamOperator<OUT>
{
private:
public:
    virtual void process_element(std::shared_ptr<StreamRecord<IN>> stream_record) {}
};
