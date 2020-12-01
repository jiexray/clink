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

    KeyedMapStateStore& window_map_state() override {
        return m_context.window_map_state();
    }

    KeyedListStateStore& window_list_state() override {
        return m_context.window_list_state();
    }

    KeyedMapStateStore& global_map_state() override {
        return m_context.global_map_state();
    }

    KeyedListStateStore& global_list_state() override {
        return m_context.global_list_state();
    }

    /** Properties */
    void set_window(W& window) {
        m_window = window;
    }

    void set_context(ProcessAllWindowFunctionContext<IN, OUT, W>& context) {
        m_context = context;
    } 
};
