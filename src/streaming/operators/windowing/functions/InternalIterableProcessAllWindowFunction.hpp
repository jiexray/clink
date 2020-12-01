#pragma once
#include "InternalWindowFunction.hpp"
#include "WrappingFunction.hpp"
#include "ProcessAllWindowFunction.hpp"
#include "InternalProcessApplyAllWindowContext.hpp"
#include <vector>

/**
  Internal window function for wrapping a ProcessAllWindowFunction that takes an Iterable
  when the window state also is an Iterable.
 */
template <class IN, class OUT, class K, class W>
class InternalIterableProcessAllWindowFunction:
    public WrappingFunction<ProcessAllWindowFunction<IN, OUT, W>>,
    public InternalWindowFunction<std::vector<IN>, OUT, K, W> {
private:
    typedef typename TemplateHelperUtil::ParamOptimize<K>::type ParamK;
    typedef typename TemplateHelperUtil::ParamOptimize<K>::const_type ConstParamK;

    InternalProcessAllWindowContext<IN, OUT, W>* m_ctx = nullptr;
public:
    InternalIterableProcessAllWindowFunction(ProcessAllWindowFunction<IN, OUT, W>& wrapped_function):
        WrappingFunction<ProcessAllWindowFunction<IN, OUT, W>>(wrapped_function) {
        this->m_ctx = new InternalProcessAllWindowContext<IN, OUT, W>();
    }

    ~InternalIterableProcessAllWindowFunction() {
        delete this->m_ctx;
    }

    void process(
            ConstParamK key, 
            const W& window, 
            InternalWindowFunctionContext& context, 
            const std::vector<IN>& input, 
            std::shared_ptr<Collector<OUT>> out) {
        this->m_ctx->set_window(window);
        this->m_ctx->set_internal_context(context);

        this->m_wrapped_function.process(*m_ctx, input, out);
    }
};

