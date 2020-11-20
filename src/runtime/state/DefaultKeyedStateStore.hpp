#pragma once

#include "KeyedMapStateStore.hpp"
#include "KeyedStateBackend.hpp"
#include "MapState.hpp"
#include "InternalMapState.hpp"
#include "ExecutionConfig.hpp"
#include "VoidNamespace.hpp"
#include <map>
/**
  Default implementation of KeyStateStore that currently forwards state registration of a RuntimeContext.
 */
template <class K, class N, typename UK, typename UV>
class DefaultKeyedMapStateStore: public KeyedMapStateStore
{
private:
    typedef typename KeyedStateBackend<K, N, std::map<UK, UV>, MapState<UK, UV>, InternalMapState<K, N, UK, UV> KeyedMapStateBackend;

    KeyedMapStateBackend& m_keyed_state_backend;
    ExecutionConfig& m_execution_config;

    void get_map_state_impl(const MapStateDescriptor<UK, UV>& state_properties, MapState<UK, UV>* ret_state) {
        ret_state = get_partitioned_state(state_properties);
    }

protected:
    MapState<UK, UV>* get_partitioned_state(const MapStateDescriptor<UK, UV>& state_descriptor) {
        return this->m_keyed_state_backend.get_partitioned_state(*VoidNamespace::instance(), state_descriptor);
    }
    
public:
    friend class MapStateGetterImpl<UK, UV, DefaultKeyedMapStateStore<UK, UV>>;

    DefaultKeyedMapStateStore(KeyedMapStateBackend& keyed_state_backend, ExecutionConfig& execution_config): 
            KeyedMapStateStore(*(new MapStateGetterImpl<UK, UV, DefaultKeyedMapStateStore<UK, UV>>(*this))),
            m_keyed_state_backend(keyed_state_backend),
            m_execution_config(execution_config) {}
    ~DefaultKeyedMapStateStore() {}
};

