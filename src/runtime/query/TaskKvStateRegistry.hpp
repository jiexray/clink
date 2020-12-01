#pragma once
#include "KvStateRegistry.hpp"
#include <vector>

/**
  A helper for KvState registration of a single task.

  @param <K> The type of key.
  @param <N> The type of namespace.
  @param <V> The type of values.
 */
template <class K, class N, class V>
class TaskKvStateRegistry
{
private:
    /**
      3-tuple holding registered KvState meta data.
     */
    struct KvStateInfo {
        KeyGroupRange   m_key_group_range;
        std::string     m_registration_name;
        KvStateID       m_kv_state_id;
        
        KvStateInfo(KeyGroupRange key_group_range, std::string registration_name, KvStateID kv_state_id):
        m_key_group_range(key_group_range), m_registration_name(registration_name), m_kv_state_id(kv_state_id) {}
    };

    KvStateRegistry<K, N, V>&       m_registry;

    int                             m_job_id;
    int                             m_job_vertex_id;

    std::vector<KvStateInfo*>       m_registered_kv_states;
public:

    TaskKvStateRegistry(KvStateRegistry<K, N, V>& registry, int job_id, int job_vertex_id):
    m_registry(registry), m_job_id(job_id), m_job_vertex_id(job_vertex_id) {}

    /**
      Registers the KvState instance at the KvStateRegistry.

      @param key_group_range    Key group range the KvState instance belongs to
      @param registration_name  The registration name 
      @param kv_state           The KvState
     */
    void register_kv_state(KeyGroupRange key_group_range, std::string registration_name, InternalKvState<K, N, V>* kv_state) {
        KvStateID kv_state_id = m_registry.register_kv_state(m_job_id, m_job_vertex_id, key_group_range, registration_name, kv_state);
        m_registered_kv_states.push_back(new KvStateInfo(key_group_range, registration_name, kv_state_id));
    }

    /**
      Unregisters all registered KvState instances from the KvStateRegistry.
     */
    void unregister_all() {
        while(!m_registered_kv_states.empty()) {
            KvStateInfo* kv_state = m_registered_kv_states.back();
            m_registry.unregisterKvState(
                m_job_id, 
                m_job_vertex_id, 
                kv_state->m_key_group_range, 
                kv_state->m_registration_name,
                kv_state->m_kv_state_id);
            delete kv_state;
            m_registered_kv_states.pop_back();
        }
    }
};

