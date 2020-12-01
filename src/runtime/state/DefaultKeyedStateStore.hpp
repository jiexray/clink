#pragma once

#include "KeyedMapStateStore.hpp"
#include "KeyedListStateStore.hpp"
#include "KeyedStateBackend.hpp"
#include "MapState.hpp"
#include "InternalMapState.hpp"
#include "ExecutionConfig.hpp"
#include "VoidNamespace.hpp"
#include <map>
#include <vector>
/**
  Default implementation of KeyedMapStateStore that currently forwards state registration of a RuntimeContext.
 */
template <class K, class N, typename UK, typename UV>
class DefaultKeyedMapStateStore: public KeyedMapStateStore
{
private:    
    typedef KeyedStateBackend<K, N, std::map<UK, UV>, MapState<UK, UV>, InternalMapState<K, N, UK, UV>> KeyedMapStateBackendType;

    KeyedMapStateBackendType& m_keyed_state_backend;
    ExecutionConfig& m_execution_config;

    void get_map_state_impl(const MapStateDescriptor<UK, UV>& state_properties, MapState<UK, UV>* ret_state) {
        ret_state = get_partitioned_state(state_properties);
    }

protected:
    virtual MapState<UK, UV>* get_partitioned_state(const MapStateDescriptor<UK, UV>& state_descriptor) {
        return this->m_keyed_state_backend.get_partitioned_state(*VoidNamespace::instance(), state_descriptor);
    }
    
public:
    friend class MapStateGetterImpl<UK, UV, DefaultKeyedMapStateStore<K, N, UK, UV>>;

    DefaultKeyedMapStateStore(KeyedMapStateBackendType& keyed_state_backend, ExecutionConfig& execution_config): 
            KeyedMapStateStore(*(new MapStateGetterImpl<UK, UV, DefaultKeyedMapStateStore<K, N, UK, UV>>(*this))),
            m_keyed_state_backend(keyed_state_backend),
            m_execution_config(execution_config) {}
    ~DefaultKeyedMapStateStore() {}
};


/**
  Default implementation of KeyedListStateStore that currently forwards state registration of a RuntimeContext.
 */
template <class K, class N, class T>
class DefaultKeyedListStateStore: public KeyedListStateStore {
private:
    typedef KeyedStateBackend<K, N, std::vector<T>, ListState<T>, InternalListState<K, N, T>> KeyedListStateBackendType;

    void get_list_state_impl(const ListStateDescriptor<T>& state_properties, ListState<T>* ret_state) {
        ret_state = &(this->get_partitioned_state(state_properties));
    }
protected:

    KeyedListStateBackendType& m_keyed_state_backend;
    ExecutionConfig& m_execution_config;

    virtual ListState<T>& get_partitioned_state(const ListStateDescriptor<T>& state_descriptor) {
        N empty_ns;
        return this->m_keyed_state_backend.get_partitioned_state(empty_ns, state_descriptor);
    }

public:
    friend class ListStateGetterImpl<T, DefaultKeyedListStateStore<K, N, T>>;

    DefaultKeyedListStateStore(KeyedListStateBackendType& keyed_state_backend, ExecutionConfig& execution_config):
        KeyedListStateStore(*(new ListStateGetterImpl<T, DefaultKeyedListStateStore<K, N, T>>(*this))),
        m_keyed_state_backend(keyed_state_backend),
        m_execution_config(execution_config) {}
    
    ~DefaultKeyedListStateStore() {}
};
