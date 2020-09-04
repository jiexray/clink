/**
 * A StreamOperator is supplied with an object of this interface that can be used to 
 * emit element and other message.
 */

#pragma once
#include "../../streamrecord/StreamRecord.hpp"
#include <memory>

template <class T>
class Output
{
public:
    virtual void collect(std::shared_ptr<StreamRecord<T>> record) = 0;
};
