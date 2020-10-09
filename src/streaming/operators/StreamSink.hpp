/**
 * A StreamOperator for executing SinkFunction.
 */
#pragma once

#include "AbstractUdfStreamOperator.hpp"
#include "../functions/SinkFunction.hpp"
#include "OneInputStreamOperator.hpp"
#include "TemplateHelper.hpp"

// NOTE: the second template parameter of AbstractUdfStreamOperator is just a place holder.
template <class IN>
class StreamSink final: public AbstractUdfStreamOperator<Function, NullType>, public OneInputStreamOperator<IN, NullType>
{
public:
    StreamSink(std::shared_ptr<SinkFunction<IN>> sink_function): AbstractUdfStreamOperator<Function, NullType>(sink_function){}

    void process_element(std::shared_ptr<StreamRecord<IN>> record) {
        // std::cout << "[DEBUG] StreamSink::process_element()" << std::endl;
        (std::dynamic_pointer_cast<SinkFunction<IN>>(this->m_user_function))->invoke(*(record->get_value()));
    }

    /* Properties */
    std::shared_ptr<SinkFunction<IN>>     get_user_function() {return std::dynamic_pointer_cast<SinkFunction<IN>>(this->m_user_function);}

    void close() {
        std::dynamic_pointer_cast<SinkFunction<IN>>(this->m_user_function)->close();
    }

    void open() {
        std::dynamic_pointer_cast<SinkFunction<IN>>(this->m_user_function)->open();
    }
};


