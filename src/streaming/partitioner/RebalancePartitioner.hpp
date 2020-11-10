/**
 * Partitioner that distribute the data equally by cycling through the output channels.
 */
#pragma once
#include "StreamPartitioner.hpp"
#include <cstdlib>
#include <ctime>

template <class T>
class RebalancePartitioner: public StreamPartitioner<T>
{
private:
    int         next_channel_to_send_to;
public:
    void        setup(int number_of_channels) override {
        StreamPartitioner<T>::setup(number_of_channels);  
        srand(time(NULL));
        next_channel_to_send_to = rand() % number_of_channels;
    }

    int         select_channel(StreamRecordV2<T>* record) override {
        next_channel_to_send_to = (next_channel_to_send_to + 1) % StreamPartitioner<T>::m_number_of_channels;
        return next_channel_to_send_to;
    }

    std::string     to_string() override {return "REBALANCE";}
};

