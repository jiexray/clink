#pragma once
#include "Function.hpp"
#include "KeyedMapStateStore.hpp"
#include "KeyedListStateStore.hpp"
#include "Collector.hpp"
#include <memory>
#include <vector>

template <class IN, class OUT, class W> 
class ProcessAllWindowFunctionContext {
    /**
         @return The window that is being evaluated.
        */
    virtual const W& window() const = 0;

    /**
         State accessor for per-key and per-window state.
        */
    virtual KeyedMapStateStore& window_map_state() = 0;

    virtual KeyedListStateStore& window_list_state() = 0;

    /**
     * State accessor for per-key global state.
     */
    virtual KeyedMapStateStore& global_map_state() = 0;

    virtual KeyedListStateStore& global_list_state() = 0;
};

/**
  Base abstract class for functions that are evaluated over non-keyed windows using a context
  for retrieving extra information.

  @param <IN> The type of the input value.
  @param <OUT> The type of the output value.
  @param <W> The type of Window that this window function can be applied on.
 */
template <class IN, class OUT, class W> 
class ProcessAllWindowFunction: public Function {
public:

    virtual void process(
            ProcessAllWindowFunctionContext<IN, OUT, W>& context, 
            const std::vector<IN>& elements, 
            std::shared_ptr<Collector<OUT>> out) = 0;

    virtual void open() = 0;
    virtual void close() = 0;
};

