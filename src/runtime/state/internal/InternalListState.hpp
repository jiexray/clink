#pragma once
#include "ListState.hpp"
#include "InternalAppendingState.hpp"
#include <vector>

/**
  The peer to the ListState in the internal state type hierarchy.
 */
template <class K, class N, class T>
class InternalListState: 
    public ListState<T>, 
    virtual public InternalAppendingState<K, N, T, std::vector<T>, std::vector<T>> {};

