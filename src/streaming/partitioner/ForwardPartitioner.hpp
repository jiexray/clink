/**
 * Partitioner that forwards element only to the first logical channel.
 */
#pragma once

#include "StreamPartitioner.hpp"

template <class T>
class ForwardPartitioner : public StreamPartitioner<T>
{
private:
    
public:
    // TODO: rewrite copy construction
    int             select_channel(StreamRecordV2<T>* record) override {return 0;}

    std::string     to_string() override {return "FORWARD";}
};

