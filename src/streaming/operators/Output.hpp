/**
 * A StreamOperator is supplied with an object of this interface that can be used to 
 * emit element and other message.
 */

#pragma once
// #include "../../streamrecord/StreamRecord.hpp"
#include "StreamRecordV2.hpp"
#include "Collector.hpp"
#include <memory>

template <class T>
class Output : public Collector<T>
{
public:
    void collect(T* val) override {
        StreamRecordV2<T>* new_record = new StreamRecordV2<T>(*val);
        collect(new_record);
        delete new_record;
    }

    virtual void    collect(StreamRecordV2<T>* record) = 0;
};
