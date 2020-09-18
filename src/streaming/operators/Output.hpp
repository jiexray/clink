/**
 * A StreamOperator is supplied with an object of this interface that can be used to 
 * emit element and other message.
 */

#pragma once
#include "../../streamrecord/StreamRecord.hpp"
#include "Collector.hpp"
#include <memory>

template <class T>
class Output : public Collector<T>
{
public:
    void            collect(std::shared_ptr<T> val) override {
        collect(std::make_shared<StreamRecord<T>>(val));
    }
    virtual void    collect(std::shared_ptr<StreamRecord<T>> record) = 0;
};
