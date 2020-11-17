#pragma once
#include "tbb/concurrent_hash_map.h"
#include "KvStateEntry.hpp"
#include "KeyGroupRange.hpp"
#include "KvStateID.hpp"
#include "LoggerFactory.hpp"

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
    tbb::concurrent_hash_map<KvStateID, KvStateEntry<K, N, V>> m_registered_kv_states;

    // TODO: implement KvStateRegistryListener
    // tbb::concurrent_hash_map<int, >

    std::shared_ptr<spdlog::logger> m_logger;
public:

    KvStateRegistry() {
        m_logger = LoggerFactory::get_logger("KvStateRegistry");
    }
    
    /**
      Registers the KvState instance and returns the assigned ID.

      @param job_id             job id the KvState instance belongs to.
      @param job_vertex_id      job vertex id the KvState instance belongs to.
      @param key_group_range    key group range the KvState instance belongs to.
      @param registration_name  name under which the KvState is registered.
      @param kv_state           KvState instance to be registered.
      @return Assigned kv_state_id
     */
    KvStateID register_kv_state(int job_id, int job_vertex_id, KeyGroupRange key_group_range, std::string registration_name, InternalKvState<K, N, V>* kv_state) {
        KvStateID kv_state_id;

        if (m_registered_kv_states.find(kv_state_id) != m_registered_kv_states.end()) {
            SPDLOG_LOGGER_WARN(m_logger, "register KvState, key {} has already exists!", kv_state_id.to_string());
        } else {
            m_registered_kv_states[kv_state_id] = KvStateEntry<K, N, V>(kv_state);
        }
        
        return kv_state_id;
    }

    /**
      Unregisters the KvState instance identified by the given KvStateId.

      @param job_id             job id KvState instance belong to
      @param kv_state_id        KvStateID to identify the KvState instance
      @param key_group_range    key group range the KvState instance belongs to
     */
    void unregisterKvState(
            int job_id, 
            int job_vertex_id, 
            KeyGroupRange key_group_range, 
            std::string registration_name,
            KvStateID kv_state_id) {
        if (m_registered_kv_states.find(kv_state_id) == m_registered_kv_states.end()) {
            throw std::runtime_error("Cannot find KvStateId: " + kv_state_id.to_string());
        }
        KvStateEntry<K, N, V> entry = m_registered_kv_states[kv_state_id];

        m_registered_kv_states.erase(kv_state_id);
    }

    /**
      Returns the KvStateEntry containing the requested instance as indentified by the
      given KvStateID.

      @param kv_state_id KvStateID to identify the KvState instance
      @return The KvStateEntry instance identified by the KvStateID.
     */
    KvStateEntry<K, V, N> get_kv_state(KvStateID kv_state_id) {
        if (m_registered_kv_states.find(kv_state_id) == m_registered_kv_states.end()) {
            throw std::runtime_error("Cannot find KvStateId: " + kv_state_id.to_string());
        }
        return m_registered_kv_states[kv_state_id];
    }


};

