/**
 * A StreamOperator for executing SinkFunction.
 */
#pragma once

#include "AbstractUdfStreamOperator.hpp"
#include "../functions/SinkFunction.hpp"
#include "OneInputStreamOperator.hpp"

// NOTE: the second template parameter of AbstractUdfStreamOperator is just a place holder.
template <class IN, class OUT = std::string>
class StreamSink final: public AbstractUdfStreamOperator<Function, OUT>, public OneInputStreamOperator<IN, std::string>
{
public:
    StreamSink(std::shared_ptr<SinkFunction<IN>> sink_function): AbstractUdfStreamOperator<Function, OUT>(sink_function){}

    void process_element(std::shared_ptr<StreamRecord<IN>> record) {
        (std::dynamic_pointer_cast<SinkFunction<IN>>(this->m_user_function))->invoke(*(record->get_value()));
    }

    /* Properties */
    std::shared_ptr<SinkFunction<IN>>     get_user_function() {return std::dynamic_pointer_cast<SinkFunction<IN>>(this->m_user_function);}
};


