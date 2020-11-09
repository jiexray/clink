/**
 * A StreamOperator for executing MapFunction.
 */
#pragma once
#include "AbstractUdfStreamOperator.hpp"
#include "../functions/MapFunction.hpp"
#include "OneInputStreamOperator.hpp"
#include <iostream>


template <class IN, class OUT>
// class StreamMap : public AbstractUdfStreamOperator<MapFunction<IN, OUT>, OUT>, public OneInputStreamOperator<IN, OUT>
class StreamMap final : public AbstractUdfStreamOperator<Function, OUT>, public OneInputStreamOperator<IN, OUT>
{
private:
public:
    StreamMap(std::shared_ptr<MapFunction<IN, OUT>> mapper): AbstractUdfStreamOperator<Function, OUT>(mapper) {}

    void process_element(StreamRecordV2<IN> *stream_record) {
        // dynamic change Function to MapFunction
        std::shared_ptr<MapFunction<IN, OUT>> map_func = std::dynamic_pointer_cast<MapFunction<IN, OUT>>(this->m_user_function);

        OUT* out_value = map_func->map(&(stream_record->val));
        if (this->m_output == nullptr) {
            throw std::runtime_error("Output is null in StreamMap");
        }
        this->m_output->collect(out_value);
        delete out_value;
    }

    // void setup(std::shared_ptr<StreamTask<OUT>> containingTask, std::shared_ptr<Output<OUT>> output) override {
    //     AbstractUdfStreamOperator<Function, OUT>::setup(containingTask, output);
    // }

    /* Properties */
    std::shared_ptr<MapFunction<IN, OUT>>   get_user_function() {return std::dynamic_pointer_cast<MapFunction<IN, OUT>>(this->m_user_function);}
};