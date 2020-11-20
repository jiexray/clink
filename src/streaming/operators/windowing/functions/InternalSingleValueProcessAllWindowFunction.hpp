#pragma once
#include "WrappingFunction.hpp"
#include "InternalWindowFunction.hpp"
#include "ProcessAllWindowFunction.hpp"
#include "InternalProcessAllWindowContext.hpp"

/**
  Internal window function for wrapping a ProcessAllWindowFunction that takes an Interable
  when the window state is a single value.
 */
template <class IN, class OUT, class W>
class InternalSingleValueProcessAllWindowFunction:
    public WrappingFunction<ProcessAllWindowFunction<IN, OUT, W>>,
    public InternalWindowFunction<IN, OUT, char, W> {
private:
    InternalProcessAllWindowContext<IN, OUT, W>* m_ctx = nullptr;
public:
    InternalSingleValueProcessAllWindowFunction(ProcessAllWindowFunction<IN, OUT, W>& wrapped_function):
        WrappingFunction<ProcessAllWindowFunction<IN, OUT, W>>(wrapped_function) {}

    void open() {
        this->m_ctx = new InternalProcessAllWindowContext<IN, OUT, W>();
    }

    void clear() {
        delete this->m_ctx;
    }

    void process(char key, W& window, InternalWindowContext& context, IN* input, std::shared_ptr<Collector<OUT>> out) {
        this->m_ctx->set_window(window);
        this->m_ctx->set_internal_context(context);

        this->m_wrapped_function.process(m_ctx, std::vector<IN*>{input}, out);
    }
};
