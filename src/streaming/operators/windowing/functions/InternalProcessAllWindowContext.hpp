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
    const W* _window = nullptr;
    InternalWindowFunctionContext* _internal_context = nullptr;
public:
    InternalProcessAllWindowContext() {
    }

    void set_window(const W& window) {
        _window = &window;
    }
    
    void set_internal_context (InternalWindowFunctionContext& internal_context) {
        _internal_context = &internal_context;
    }

    const W& window() const override{
        assert(_window != nullptr);
        return *_window;
    }

    KeyedMapStateStore& window_map_state() override {
        assert(_internal_context != nullptr);
        return _internal_context->window_map_state();
    }

    KeyedListStateStore& window_list_state() override {
        assert(_internal_context != nullptr);
        return _internal_context->window_list_state();
    }

    KeyedMapStateStore& global_map_state() override {
        assert(_internal_context != nullptr);
        return _internal_context->global_map_state();
    }

    KeyedListStateStore& global_list_state() override {
        assert(_internal_context != nullptr);
        return _internal_context->global_list_state();
    }
};