#pragma once

#include "InternalKvState.hpp"
#include "MapState.hpp"
#include <map>

/**
  The peer to the MapState in the internal state type hierarchy.

  @param <K>: type of key
  @param <N>: type of ns
  @param <UK>: Type of the values folded into the state
  @param <UV>: Type of the value in the state
 */
template <class K, class N, class UK, class UV>
class InternalMapState: virtual public InternalKvState<K, N, std::map<UK, UV>>, virtual public MapState<UK, UV>
{};

