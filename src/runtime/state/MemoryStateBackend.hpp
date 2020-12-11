#pragma once

#include "StateBackend.hpp"
#include "HeapKeyedStateBackend.hpp"
#include "InternalKeyContextImpl.hpp"

/**
  This state backend holds the working state in the memory of the TaskManager.
 */
template <class K, class N, class SV, class S, class IS>
class MemoryStateBackend: public StateBackend<K, N, SV, S, IS> {
public:
    AbstractKeyedStateBackend<K, N, SV, S, IS>* create_keyed_state_backend (
            ExecutionConfig& execution_config,
            int number_of_key_groups,
            const KeyGroupRange& key_group_range) override {
            
        InternalKeyContext<K>* key_context = new InternalKeyContextImpl<K>(key_group_range, number_of_key_groups);
        return new HeapKeyedStateBackend<K, N, SV, S, IS>(
            execution_config,
            key_context,
            std::map<std::string, StateTable<K, N, SV>*>());
    }
};
