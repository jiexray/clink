#pragma once

#include "InternalAppendingState.hpp"
#include "AggregatingState.hpp"

/**
  The peer to the AggregatingState in the internal state type hierarchy.
 */
template <class K, class N, class IN, class SV, class OUT> 
class InternalAggregatingState: 
        public AggregatingState<IN, OUT>, 
        virtual public InternalAppendingState<K, N, IN, SV, OUT> {
};

