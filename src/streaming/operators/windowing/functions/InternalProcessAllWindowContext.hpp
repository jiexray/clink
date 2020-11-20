#pragma once
#include "ProcessAllWindowFunction.hpp"
#include "InternalWindowFunction.hpp"

/**
  Internal reusable context wrapper.

  @param <IN> The type of the input value.
  @param <OUT> The type of the output value.
  @param <W> The type of the window.
 */
template <class IN, class OUT, class W>
class InternalProcessAllWindowContext: public ProcessAllWindowFunctionContext<IN, OUT, W>
{
private:
    W& m_window;
    InternalWindowContext& m_internal_context;
public:
    InternalProcessAllWindowContext() {

    }

    void set_window(W& window) {
        m_window = window;
    }
    
    void set_internal_context (InternalWindowContext& internal_context) {
        m_internal_context = internal_context;
    }

    W& window() override{
        return m_window;
    }

    KeyedMapStateStore& window_state() override {
        return m_internal_context.window_state();
    }

    KeyedMapStateStore& global_state() override {
        return m_internal_context.global_state();
    }
};