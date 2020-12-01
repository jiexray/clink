#pragma once

#include "AbstractKeyedStateBackend.hpp"
#include "DefaultKeyedStateStore.hpp"
#include "TemplateHelper.hpp"
#include <iostream>

/**
  Class encapsulating various state backend handling logic for StreamOperator implementation
 */
class StreamOperatorStateHandler
{
public:

};

template <class K, class N, class SV, class S, class IS>
class StreamOperatorStateHandlerImpl;

template <class K, class N, class UK, class UV>
class StreamOperatorStateHandlerImpl<K, N, std::map<UK, UV>, MapState<UK, UV>, InternalMapState<K, N, UK, UV>>: public StreamOperatorStateHandler{
private:
    typedef AbstractKeyedStateBackend<K, N, std::map<UK, UV>, MapState<UK, UV>, InternalMapState<K, N, UK, UV>> AbstractKeyedStateBackendType;
    typedef typename TemplateHelperUtil::ParamOptimize<N>::type ParamN;
    typedef typename TemplateHelperUtil::ParamOptimize<N>::const_type ConstParamN;
    typedef typename TemplateHelperUtil::ParamOptimize<K>::type ParamK;
    typedef typename TemplateHelperUtil::ParamOptimize<K>::const_type ConstParamK;

    AbstractKeyedStateBackendType& m_keyed_state_backend;
    DefaultKeyedMapStateStore<K, N, UK, UV>* m_keyed_state_store;
    ExecutionConfig& m_execution_config;
public:
    StreamOperatorStateHandlerImpl(
            AbstractKeyedStateBackendType& keyed_state_backend,
            ExecutionConfig& execution_config): 
            m_keyed_state_backend(keyed_state_backend),
            m_execution_config(execution_config) {
        m_keyed_state_store = new DefaultKeyedMapStateStore<K, N, UK, UV>(m_keyed_state_backend, m_execution_config);
    }
    ~StreamOperatorStateHandlerImpl() {
        delete m_keyed_state_store;
    }

    MapState<UK, UV>& get_or_create_keyed_state(const StateDescriptor<MapState<UK, UV>, std::map<UK, UV>>& state_descriptor) {
        return this->m_keyed_state_backend.get_or_create_keyed_state(state_descriptor);
    }

    MapState<UK, UV>& get_partitioned_state(ConstParamN ns, const StateDescriptor<MapState<UK, UV>, std::map<UK, UV>>& state_descriptor) {
        return this->m_keyed_state_backend.get_partitioned_state(ns, state_descriptor);
    }

    void set_current_key(ConstParamK key) {
        this->m_keyed_state_backend.set_current_key(key);
    }

    ConstParamK get_current_key() {
        return this->m_keyed_state_backend.get_current_key();
    }

    DefaultKeyedMapStateStore<K, N, UK, UV>& get_keyed_state_store() {
        return *(this->m_keyed_state_store);
    }

    AbstractKeyedStateBackendType& get_keyed_state_backend() {
        return this->m_keyed_state_backend;
    }

    ExecutionConfig& get_execution_config() {
        return m_execution_config;
    }
};

template <class K, class N, class T>
class StreamOperatorStateHandlerImpl<K, N, std::vector<T>, ListState<T>, InternalListState<K, N, T>>: public StreamOperatorStateHandler {
private:
    typedef AbstractKeyedStateBackend<K, N, std::vector<T>, ListState<T>, InternalListState<K, N, T>> AbstractKeyedStateBackendType;
    typedef typename TemplateHelperUtil::ParamOptimize<N>::type ParamN;
    typedef typename TemplateHelperUtil::ParamOptimize<N>::const_type ConstParamN;
    typedef typename TemplateHelperUtil::ParamOptimize<K>::type ParamK;
    typedef typename TemplateHelperUtil::ParamOptimize<K>::const_type ConstParamK;

    AbstractKeyedStateBackendType& m_keyed_state_backend;
    DefaultKeyedListStateStore<K, N, T>* m_keyed_state_store;
    ExecutionConfig& m_execution_config;
public:
    StreamOperatorStateHandlerImpl(
            AbstractKeyedStateBackendType& keyed_state_backend, 
            ExecutionConfig& execution_config):
            m_keyed_state_backend(keyed_state_backend),
            m_execution_config(execution_config) {
        m_keyed_state_store = new DefaultKeyedListStateStore<K, N, T>(keyed_state_backend, m_execution_config);
    }

    ~StreamOperatorStateHandlerImpl() {
        delete m_keyed_state_store;
    }

    ListState<T>& get_or_create_keyed_state(const StateDescriptor<ListState<T>, std::vector<T>>& state_descriptor) {
        // if (this->m_keyed_state_backend)
        return (this->m_keyed_state_backend).get_or_create_keyed_state(state_descriptor);
    }

    ListState<T>& get_partitioned_state(ConstParamN ns, const StateDescriptor<ListState<T>, std::vector<T>>& state_descriptor) {
        return this->m_keyed_state_backend.get_partitioned_state(ns, state_descriptor);
    }

    void set_current_key(ConstParamK key) {
        this->m_keyed_state_backend.set_current_key(key);
    }

    ConstParamK get_current_key() {
        return this->m_keyed_state_backend.get_current_key();
    }

    DefaultKeyedListStateStore<K, N, T>& get_keyed_state_store() {
        return *(this->m_keyed_state_store);
    }

    AbstractKeyedStateBackendType& get_keyed_state_backend() {
        return this->m_keyed_state_backend;
    }

    ExecutionConfig& get_execution_config() {
        return m_execution_config;
    }
};
