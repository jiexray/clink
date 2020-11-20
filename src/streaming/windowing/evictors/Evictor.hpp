#pragma once
#include <vector>
#include "TimestampedValue.hpp"

/**
  A context object that is given to Evictor methods
 */
class EvictorContext
{
public:
    virtual long get_current_processing_time() = 0;

    virtual long get_current_watermark() = 0;
};


/**
  A Evictor can remove elements form a pane before/after the evaluation of WindowFunction
  and after the window evaluation gets triggered by a Trigger.

  @param <T> The type of elements that this Evictor can evict
  @param <W> The type of Window on which this Evictor can operate
 */
template <class T, class W>
class Evictor
{
public:
    /**
      Optionally evicts elements. Called before windowing function.
     */
    virtual void evict_before(
            std::vector<TimestampedValue<T>> elements, 
            int size, 
            W& window, 
            EvictorContext& evictor_context) = 0

    /**
      Optionally evicts elements. Called after windowing function.
     */
    virtual void evict_after(
            std::vector<TimestampedValue<T>> elements, 
            int size, 
            W& window, 
            EvictorContext& evictor_context) = 0

};
