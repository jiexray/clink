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
    InternalKvState<K, N, V>* m_state;
    
public:
    KvStateEntry(InternalKvState<K, N, V>* state): m_state(state) {}

    KvStateEntry(const KvStateEntry& other) {
        m_state = other.m_state;
    }

    InternalKvState<K, N, V>& get_state() {
        return m_state;
    }

    KvStateEntry& operator= (const KvStateEntry& other) {
        if (this != &other) {
            this->m_state = other.m_state;
        }
        return *this;
    }

    KvStateEntry& operator= (KvStateEntry&& other) {
        if (this != &other) {
            this->m_state = other.m_state;
        }
        return *this;
    }
};

