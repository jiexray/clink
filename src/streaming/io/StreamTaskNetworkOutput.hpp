/**
 * The network data output implementation used for processing data from 
 * StreamTaskNetworkInput in one input processor.
 */
#pragma once

#include "DataOutput.hpp"
#include "StreamOperator.hpp"
#include "OneInputStreamOperator.hpp"
#include "Counter.hpp"
#include <memory>
#include <iostream>

template <class IN, class OUT>
class StreamTaskNetworkOutput : public DataOutput<IN>
{
private:
    typedef std::shared_ptr<Counter>                             CounterPtr;

    std::shared_ptr<OneInputStreamOperator<IN, OUT>>             m_operator;

    CounterPtr                                                   m_num_records_in;

    
public:
    StreamTaskNetworkOutput(std::shared_ptr<OneInputStreamOperator<IN, OUT>> op) {
        this->m_operator = op;
        this->m_num_records_in = nullptr;
    }

    void set_num_records_in(CounterPtr counter) override {m_num_records_in = counter;}

    void emit_record(StreamRecordV2<IN>* stream_record) {
        this->m_num_records_in->inc();
        this->m_operator->process_element(stream_record);
    }

    void close() override {
        m_operator->close();
    }
};

