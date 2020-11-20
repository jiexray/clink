#pragma once
#include "Function.hpp"
#include "Collector.hpp"
#include <memory>
#include <vector>

/**
  Base interface for functions that are evaluated over non-keyed windows.

  @param <IN> the type of the input value.
  @param <OUT> the type of the output value.
  @param <W> the type of Window that this window function can be applied on.
 */
template <class IN, class OUT, class W>
class AllWindowFunction : public {
public:
    /**
      Evaluated the window and outputs none or several elements.

      @param window The window that is being evaluated.
      @param values The elements in the window being evaluated.
      @param out A collector for emitting elements.
     */
    virtual void apply(W* window, const std::vector<IN*>& values, std::shared_ptr<Collector<OUT>> collector) = 0;
};
