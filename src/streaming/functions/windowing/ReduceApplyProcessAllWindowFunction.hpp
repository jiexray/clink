#pragma once
#include "ProcessAllWindowFunction.hpp"
#include "ReduceFunction.hpp"
#include "ProcessAllWindowFunction.hpp"
#include "InternalProcessApplyAllWindowContext.hpp"

/**
  Internal ProcessAllWindowFunction that is used for implementing a fold on a window
  configuration that only allows AllWindowFunction and cannot directly execute a ReduceFunction.
 */
template <class W, class T, class R>
class ReduceApplyProcessAllWindowFunction: public ProcessAllWindowFunction<T, R, W>
{
private:
    ReduceFunction<T>& m_reduce_function;
    ProcessAllWindowFunction<T, R, W>& m_window_function;

    InternalProcessApplyAllWindowContext<T, R, W>* m_ctx;

public:
    ReduceApplyProcessAllWindowFunction(ReduceFunction<T>& reduce_function, ProcessAllWindowFunction<T, R, W>& window_function):
            m_reduce_function(reduce_function), m_window_function(window_function) {}

    void process(
            ProcessAllWindowFunctionContext<IN, OUT, W>& context, 
            std::vector<T>& elements, 
            std::shared_ptr<Collector<R>> out) override {
        T* curr = nullptr;
        for (T val: elements) {
            if (curr == nullptr) {
                curr = val;
            } else {
                curr = this->m_reduce_function.reduce(curr, val);
            }
        }

        this->m_ctx->set_context(context);
        this->m_ctx->set_window(context.window());

        m_window_function.process(m_ctx, std::vector<T>{*curr}, out);
    }

    void close() override {
        delete m_ctx;
    }

    void open() override {
        m_ctx = new InternalProcessApplyAllWindowContext<T, R, W>();
    }

};

