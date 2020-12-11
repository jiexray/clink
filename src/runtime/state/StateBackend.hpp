#pragma once

#include <memory>
#include "AbstractKeyedStateBackend.hpp"
#include "KeyGroupRange.hpp"

/**
  A state backend defines how the state of a streaming application is stored and
   checkpointed. Different State Backends store their state in different fashions, and use
   different data structures to hold the state of a running application.
 */
template <class K, class N, class SV, class S, class IS>
class StateBackend {
public:
    /**
      Create a new AbstractKeyedStateBackend that is responsible for holding keyed state
      and checkpointing it.

      @param env The environment of the task.
      @param job_id The ID of the job that the task belongs to.
      @param operator_identifier The identifier text of the operator.
      @param number_of_key_groups The number of key-groups aka max parallelism.
      @param key_group_range Range of key-groups for which the to-be-created backend is reponsible.
     */
    virtual AbstractKeyedStateBackend<K, N, SV, S, IS>* create_keyed_state_backend(
            ExecutionConfig& execution_config,
            int number_of_key_groups,
            const KeyGroupRange& key_group_range) = 0;
};
