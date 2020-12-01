#pragma once
#include "AppendingState.hpp"

/**
  State interface for aggregating state, based onan 
  AggregateFunction. Elements that are added to this type of state will
  be eagerly pre-aggregated using a given aggregateFunction.
 */
template <class IN, class OUT>
class AggregatingState: public AppendingState<IN, OUT> {
};

