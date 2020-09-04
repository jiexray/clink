/**
 * A StreamOperator for executing MapFunction.
 */
#pragma once
#include "AbstractUdfStreamOperator.hpp"
#include "../functions/MapFunction.hpp"
#include "OneInputStreamOperator.hpp"
#include <iostream>

template <class OUT> class StreamTask;

template <class IN, class OUT>
// class StreamMap : public AbstractUdfStreamOperator<MapFunction<IN, OUT>, OUT>, public OneInputStreamOperator<IN, OUT>
class StreamMap final : public AbstractUdfStreamOperator<Function, OUT>, public OneInputStreamOperator<IN, OUT>
{
private:
public:
    StreamMap(std::shared_ptr<MapFunction<IN, OUT>> mapper): AbstractUdfStreamOperator<Function, OUT>(mapper) {}

    void process_element(std::shared_ptr<StreamRecord<IN>> stream_record) {
        std::shared_ptr<IN> record_value = stream_record->get_value();
        // dynamic change Function to MapFunction
        std::shared_ptr<MapFunction<IN, OUT>> map_func = std::dynamic_pointer_cast<MapFunction<IN, OUT>>(this->m_user_function);
        std::shared_ptr<StreamRecord<OUT>> out_stream_record = stream_record->replace(map_func->map(*(record_value.get())));
        // std::shared_ptr<StreamRecord<OUT>> out_stream_record = stream_record->replace(this->m_user_function->map(*(record_value.get())));
        std::cout << *(stream_record->get_value().get()) << "map to: " << *(out_stream_record->get_value().get()) << std::endl;
        // this->m_output->collect(out_stream_record);
    }

    void setup(std::shared_ptr<StreamTask<OUT>> containingTask, std::shared_ptr<Output<OUT>> output) override {
        AbstractUdfStreamOperator<Function, OUT>::setup(containingTask, output);
    }

    /* Properties */
    std::shared_ptr<MapFunction<IN, OUT>>   get_user_function() {return std::dynamic_pointer_cast<MapFunction<IN, OUT>>(this->m_user_function);}
};