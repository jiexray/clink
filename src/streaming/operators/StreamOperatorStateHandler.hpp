#pragma once

#include "AbstractKeyedStateBackend.hpp"
#include "DefaultKeyedStateStore.hpp"
#include "TemplateHelper.hpp"
#include <iostream>

/**
  Class encapsulating various state backend handling logic for StreamOperator implementation
 */
template <class K, class N, class SV, class S, class IS>
class StreamOperatorStateHandler {
private:
    typedef typename TemplateHelperUtil::ParamOptimize<N>::type ParamN;
    typedef typename TemplateHelperUtil::ParamOptimize<N>::const_type ConstParamN;
    typedef typename TemplateHelperUtil::ParamOptimize<K>::type ParamK;
    typedef typename TemplateHelperUtil::ParamOptimize<K>::const_type ConstParamK;
    
    AbstractKeyedStateBackend<K, N, SV, S, IS>& m_keyed_state_backend;
    ExecutionConfig& m_execution_config;

public:
    StreamOperatorStateHandler(AbstractKeyedStateBackend<K, N, SV, S, IS>& keyed_state_backend, ExecutionConfig& execution_config):
            m_keyed_state_backend(keyed_state_backend), m_execution_config(execution_config) {}
    
    S& get_or_create_keyed_state(const StateDescriptor<S, SV>& state_descriptor) {
        return this->m_keyed_state_backend.get_or_create_keyed_state(state_descriptor);
    }

    S& get_partitioned_state(ConstParamN ns, const StateDescriptor<S, SV>& state_descriptor) {
        return this->m_keyed_state_backend.get_partitioned_state(ns, state_descriptor);
    }

    void set_current_key(ConstParamK key) {
        this->m_keyed_state_backend.set_current_key(key);
    }

    ConstParamK get_current_key() {
        return this->m_keyed_state_backend.get_current_key();
    }

    AbstractKeyedStateBackend<K, N, SV, S, IS>& get_keyed_state_backend() {
        return this->m_keyed_state_backend;
    }

    ExecutionConfig& get_execution_config() {
        return m_execution_config;
    }
};

