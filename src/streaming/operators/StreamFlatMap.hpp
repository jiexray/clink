/**
 * A StreamOperator for executing FlatMapFunction.
 */
#pragma once
#include "AbstractUdfStreamOperator.hpp"
#include "FlatMapFunction.hpp"

template <class IN, class OUT> class OneInputStreamOperator;

template <class IN, class OUT>
class StreamFlatMap: public AbstractUdfStreamOperator<Function, OUT>, OneInputStreamOperator<IN, OUT>
{
private:
public:
    StreamFlatMap(std::shared_ptr<FlatMapFunction<IN, OUT>> flat_mapper): AbstractUdfStreamOperator<Function, OUT>(flat_mapper) {}

    void                                        process_element(StreamRecordV2<IN>* element) {
        (std::dynamic_pointer_cast<FlatMapFunction<IN, OUT>>(this->m_user_function))->flat_map(&(element->val), this->m_output);
    }

    std::shared_ptr<FlatMapFunction<IN, OUT>>   get_user_function() {return std::dynamic_pointer_cast<FlatMapFunction<IN, OUT>>(this->m_user_function);}
};
