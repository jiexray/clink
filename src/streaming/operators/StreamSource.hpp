/**
 * StreamOperator for streaming sources.
 */
#pragma once
#include "../functions/SourceFunction.hpp"
#include "AbstractUdfStreamOperator.hpp"
#include "StreamSourceContexts.hpp"

template <class OUT>
class StreamSource : public AbstractUdfStreamOperator<Function, OUT> {
private:
    std::shared_ptr<SourceContext<OUT>>     m_ctx;
public:
    StreamSource(std::shared_ptr<SourceFunction<OUT>> source_function): AbstractUdfStreamOperator<Function, OUT>(source_function) {}

    void                                    run() {
        // TODO: set operator configuration
        this->m_ctx = StreamSourceContexts::get_source_context<OUT>(TimeCharacteristic::ProcessingTime, this->m_output);

        (std::dynamic_pointer_cast<SourceFunction<OUT>>(this->m_user_function))->run(this->m_ctx);
    }

    /* Properties */
    std::shared_ptr<SourceFunction<OUT>>    get_user_function() {return std::dynamic_pointer_cast<SourceFunction<OUT>>(this->m_user_function);}
};
