/**
 * The network data output implementation used for processing data from 
 * StreamTaskNetworkInput in one input processor.
 */
#pragma once

#include "DataOutput.hpp"
#include "../operators/StreamOperator.hpp"
#include "../operators/OneInputStreamOperator.hpp"
#include <memory>
#include <iostream>

template <class IN, class OUT>
class StreamTaskNetworkOutput : public DataOutput<IN>
{
private:
    std::shared_ptr<OneInputStreamOperator<IN, OUT>>             m_operator;
public:
    StreamTaskNetworkOutput(std::shared_ptr<OneInputStreamOperator<IN, OUT>> op) {this->m_operator = op;}

    void emit_record(std::shared_ptr<StreamRecord<IN>> stream_record) {
        // std::cout << "TODO: deliver the stream_record to operator" << std::endl;
        // std::cout << "value: " << *(stream_record->get_value().get()) << std::endl;
        this->m_operator->process_element(stream_record);
    }
};

