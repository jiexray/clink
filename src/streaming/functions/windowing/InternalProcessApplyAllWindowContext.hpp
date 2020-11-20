#pragma once
#include "ProcessAllWindowFunction.hpp"

/**
  Internal reusable context wrapper.
 */
template <class IN, class OUT, class W>
class InternalProcessApplyAllWindowContext: public ProcessAllWindowFunctionContext<IN, OUT, W>
{
private:
    W& m_window;
    ProcessAllWindowFunctionContext<IN, OUT, W>& m_context;
public:

    InternalProcessApplyAllWindowContext() {

    }

    W& window() const override {
        return m_window;
    }

    KeyedStateMapStore& window_state() {
        return m_context.window_state();
    }

    KeyedMapStateStore& global_state() {
        return m_context.global_state();
    }

    /** Properties */
    void set_window(W& window) {
        m_window = window;
    }

    void set_context(ProcessAllWindowFunctionContext<IN, OUT, W>& context) {
        m_context = context;
    } 
};
