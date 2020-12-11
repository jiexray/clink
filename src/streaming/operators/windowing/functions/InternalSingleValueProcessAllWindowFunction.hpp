#pragma once
#include "WrappingFunction.hpp"
#include "InternalWindowFunction.hpp"
#include "ProcessAllWindowFunction.hpp"
#include "InternalProcessAllWindowContext.hpp"
#include <memory>
#include <functional>

/**
  Internal window function for wrapping a ProcessAllWindowFunction that takes an Interable
  when the window state is a single value.
 */
template <class IN, class OUT, class K, class W>
class InternalSingleValueProcessAllWindowFunction:
    public WrappingFunction<ProcessAllWindowFunction<IN, OUT, W>>,
    public InternalWindowFunction<IN, OUT, K, W> {
private:
    typedef typename TemplateHelperUtil::ParamOptimize<K>::type ParamK;
    typedef typename TemplateHelperUtil::ParamOptimize<K>::const_type ConstParamK;
    typedef std::function<ProcessAllWindowFunction<IN, OUT, W>*(void)> ProcessAllWindowFunctionCreator;

    InternalProcessAllWindowContext<IN, OUT, W>* m_ctx = nullptr;
public:
    InternalSingleValueProcessAllWindowFunction(ProcessAllWindowFunction<IN, OUT, W>& wrapped_function):
        WrappingFunction<ProcessAllWindowFunction<IN, OUT, W>>(wrapped_function) {
        this->m_ctx = new InternalProcessAllWindowContext<IN, OUT, W>();
    }

    ~InternalSingleValueProcessAllWindowFunction() {
        delete this->m_ctx;
    }

    void process(
            ConstParamK key, 
            const W& window, 
            InternalWindowFunctionContext& context, 
            const IN& input, 
            std::shared_ptr<Collector<OUT>> out) {
        this->m_ctx->set_window(window);
        this->m_ctx->set_internal_context(context);

        this->m_wrapped_function.process(*m_ctx, std::vector<IN>{input}, out);
    }

    static std::shared_ptr<InternalWindowFunction<IN, OUT, K, W>> create(ProcessAllWindowFunctionCreator func_creator) {
        return std::make_shared<InternalSingleValueProcessAllWindowFunction<IN, OUT, K, W>>(*func_creator());
    }
};
