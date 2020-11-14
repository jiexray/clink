#pragma once
#include "StateTable.hpp"
#include "NestedStateMap.hpp"

/**
  This implementation of StateTable use NestedStateMap.

  @param <K> type of key
  @param <N> type of namespace
  @param <S> type of state
 */
template <class K, class N, class S>
class NestedMapsStateTable: public StateTable<K, N, S>
{
public:
    NestedMapsStateTable(InternalKeyContext<K>* key_context): StateTable<K, N, S>(key_context) {
        for (int i = 0; i < this->m_key_grouped_state_maps_length; i++) {
            this->m_key_grouped_state_maps[i] = create_state_map();
        }
    }

    NestedStateMap<K, N, S>* create_state_map() override {
        return new NestedStateMap<K, N, S>();
    }
};

