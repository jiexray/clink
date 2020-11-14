#pragma once
#include "InternalKvState.hpp"

/**
  An entry holding the InternalKvState.

  @param <K> The type of key.
  @param <N> The type of namespace.
  @param <V> The type of values.
 */
template <class K, class N, class V>
class KvStateEntry
{
private:
    InternalKvState<K, N, V>& m_state;
    
public:
    KvStateEntry(InternalKvState<K, N, V>& state): m_state(state) {}

    InternalKvState<K, N, V>& get_state() {
        return m_state;
    }
};

