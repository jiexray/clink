#pragma once
#include "KeyedMapStateStore.hpp"
#include <memory>
#include "Collector.hpp"

/**
  A context for InternalWindowFunction
 */
class InternalWindowContext {
    virtual long current_process_time() = 0;

    virtual long current_watermark() = 0;

    virtual KeyedMapStateStore& window_state() = 0;

    virtual KeyedMapStateStore& global_state() = 0;
};

/**
  Internal interface for functions that are evaluated over keyed (grouped) windows.

  @param <IN> The type of the input value.
  @param <OUT> The type of the output value.
  @param <KEY> The type of the key.
 */
template <class IN, class OUT, class KEY, class W>
class InternalWindowFunction {
public:
    virtual void process(KEY& key, W& window, InternalWindowContext& context, IN* input, std::shared_ptr<Collector<OUT>> out) = 0;
};

