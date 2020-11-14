#pragma once
#include "tbb/concurrent_hash_map.h"
#include "KvStateEntry.hpp"
#include "KeyGroupRange.hpp"
#include "KvStateID.hpp"

/**
  A registry for InternalKvState instances per task manager.

  This is currently only used for KvState queries: KvState instances, which
  are marked as queryable in their state descriptor are registered here and
  can be queried by the KvStateServer.

  KvState is registered when it is created/restored and unregistered when
  the owning operator stops running.

  @param <K> The type of key.
  @param <N> The type of namespace.
  @param <V> The type of values.
 */
template<class K, class N, class V>
class KvStateRegistry {
private:
    tbb::concurrent_hash_map<KvStateID, KvStateEntry<K, N, V>*> registered_kv_states;

    // TODO: implement KvStateRegistryListener
    // tbb::concurrent_hash_map<int, >
public:
    
    /**
      Registers the KvState instance and returns the assigned ID.

      @param job_id             job id the KvState instance belongs to.
      @param job_vertex_id      job vertex id the KvState instance belongs to.
      @param key_group_range    key group range the KvState instance belongs to.
      @param registration_name  name under which the KvState is registered.
      @param kv_state           KvState instance to be registered.
      @return Assigned kv_state_id
     */
    KvStateID register_kv_state(int job_id, int job_vertex_id, KeyGroupRange key_group_range, std::string registration_name, InternalKvState<K, N, V> kv_state) {
        
    }


};

