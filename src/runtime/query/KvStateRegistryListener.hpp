#pragma once
#include "KeyGroupRange.hpp"

/**
  A listener for A KvStateRegistry.
 */
class KvStateRegistryListener
{
public:
    virtual void notify_kv_state_registered(
            int job_id, 
            int job_vertex_id,
            KeyGroupRange key_group_range,
            std::string registration_name,
            int kv_state_id) = 0;

    virtual void notify_kv_state_unregistered(
            int job_id, 
            int job_vertex_id,
            KeyGroupRange key_group_range,
            std::string registration_name) = 0;
};

