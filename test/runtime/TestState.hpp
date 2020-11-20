#pragma once

#include "cxxtest/TestSuite.h"
#include "NestedStateMap.hpp"
#include "NestedMapsStateTable.hpp"
#include "InternalKeyContextImpl.hpp"
#include "HeapMapState.hpp"
#include "KeyGroupRangeAssignment.hpp"
#include "HeapKeyedStatedBackend.hpp"
#include "MapStateDescriptor.hpp"
#include "KeyedMapStateStore.hpp"
#include "HeapListState.hpp"
#include "ListStateDescriptor.hpp"
#include <iostream>
#include <string>
#include <map>
#include <typeinfo>

struct MyState{
    int f0;
    int f1;
    MyState() {}
    MyState(const MyState & state) {
        f0 = state.f0;
        f1 = state.f1;
    }
    MyState(int ff0, int ff1): f0(ff0), f1(ff1){}
};

class TestState: public CxxTest::TestSuite
{
public:
    void testStateMap() {
        std::cout << "test testStateMap()" << std::endl;
        NestedStateMap<int, std::string, int> state_map;
        state_map.put(10, "ns-1", 199);

        int state_1 = state_map.get(10, "ns-1");
        TS_ASSERT_EQUALS(state_1, 199);

        NestedStateMap<int, std::string, MyState> state_map_2;
        state_map_2.put(10, "ns_1", MyState(100, 101));
        state_map_2.put(10, "ns_1", MyState(10, 11));

        MyState state_2 = state_map_2.get(10, "ns_1");
        TS_ASSERT_EQUALS(state_2.f0, 10);
        TS_ASSERT_EQUALS(state_2.f1, 11);
    }

    void testStateTable() {
        std::cout << "test testStateTable()" << std::endl;
        InternalKeyContext<int>* key_context = new InternalKeyContextImpl<int>(KeyGroupRange(0, 10), 3);
        NestedMapsStateTable<int, std::string, int> state_table(key_context);

        key_context->set_current_key(101);

        state_table.put("ns-1", 101);
        state_table.put("ns-1", 102);

        int state_1 = state_table.get("ns-1");
        TS_ASSERT_EQUALS(state_1, 102);
    }

    void testHeapState() {
        std::cout << "test testHeapState()" << std::endl;
        InternalKeyContext<int>* key_context = new InternalKeyContextImpl<int>(KeyGroupRange(0, 10), 3);
        NestedMapsStateTable<int, std::string, std::map<int, int>> state_table(key_context);

        HeapMapState<int, std::string, int, int> heap_map_state(state_table, std::map<int, int>());

        heap_map_state.set_current_namespace("ns-1");

        heap_map_state.put(10, 1);
        heap_map_state.put(12, 1);
        
        int state = heap_map_state.get(10);
        TS_ASSERT_EQUALS(state, 1);

        heap_map_state.put(12, 12);
        state = heap_map_state.get(12);
        TS_ASSERT_EQUALS(state, 12);

        heap_map_state.put_all(std::map<int, int>{{10, 100}, {12, 120}, {14, 140}});
        state = heap_map_state.get(10);
        TS_ASSERT_EQUALS(state, 100);
        state = heap_map_state.get(12);
        TS_ASSERT_EQUALS(state, 120);
        state = heap_map_state.get(14);
        TS_ASSERT_EQUALS(state, 140);
    }

    void testKeyGroupAssignment( void ) {
        std::cout << "test testKeyGroupAssignment()" << std::endl;
        int key_group_1 = KeyGroupRangeAssignment::assign_to_key_group<int>(123, 10);
        std::cout << "key_group_1 : " << key_group_1 << std::endl;
        int key_group_2 = KeyGroupRangeAssignment::assign_to_key_group<int>(122, 10);
        std::cout << "key_group_2 : " << key_group_2 << std::endl;
        TS_ASSERT_EQUALS(key_group_1 == key_group_2, false);

        int key_group_3 = KeyGroupRangeAssignment::assign_to_key_group<std::string>("hello world", 10);
        std::cout << "key_group_3 : " << key_group_3 << std::endl;
    }

    void testHeapStateBackend( void ) {
        std::cout << "test testHeapStateBackend()" << std::endl;

        KvStateRegistry<int, std::string, std::map<int, int>> kv_state_registry;
        int job_id = 0;
        int job_vertex_id = 0;

        TaskKvStateRegistry<int, std::string, std::map<int, int>> task_kv_state_registry(&kv_state_registry, job_id, job_vertex_id);
        ExecutionConfig execution_config;
        InternalKeyContext<int>* key_context = new InternalKeyContextImpl<int>(KeyGroupRange(0, 10), 3);

        HeapKeyedStatedBackend<int, std::string, std::map<int, int>, MapState<int, int>, InternalMapState<int, std::string, int, int>> heap_stated_backend(
                &task_kv_state_registry, 
                &execution_config,
                key_context,
                std::map<std::string, StateTable<int, std::string, std::map<int, int>>*>());
    

        heap_stated_backend.register_state_creator(
            // std::string(typeid(HeapMapState<int, std::string, int, int>).name()),
            std::string(typeid(StateDescriptor<MapState<int, int>, std::map<int, int>>).name()) ,
            HeapMapState<int, std::string, int, int>::create<InternalMapState<int, std::string, int, int>>);

        MapStateDescriptor<int, int> map_state_desc("map-state");
        InternalMapState<int, std::string, int, int>* map_state = heap_stated_backend.create_internal_state(map_state_desc);
        map_state->set_current_namespace("ns-1");
        (dynamic_cast<HeapMapState<int, std::string, int, int>*>(map_state))->set_current_namespace("ns-1");

        map_state->put(1, 1);
        map_state->put(2, 2);

        int state = map_state->get(1);
        TS_ASSERT_EQUALS(state, 1);
        state = map_state->get(2);
        TS_ASSERT_EQUALS(state, 2);

        map_state->put(1, 10);
        state = map_state->get(1);
        TS_ASSERT_EQUALS(state, 10);
        map_state->set_current_namespace("ns_2");
        TS_ASSERT_EQUALS(map_state->contains(1), false);

        MapStateDescriptor<int, int> map_state_desc_2("map-state2");
        MapState<int, int>& map_state_2 = heap_stated_backend.get_or_create_keyed_state(map_state_desc_2);
        InternalMapState<int, std::string, int, int>& internal_map_state_2 = heap_stated_backend.get_or_create_internal_keyed_state(map_state_desc_2);
        internal_map_state_2.set_current_namespace("ns-1");
        // ((HeapMapState<int, std::string, int, int>*)(&map_state_2))->set_current_namespace("ns-1");

        // (dynamic_cast<InternalMapState<int, std::string, int, int>*>(&map_state_2))->put(1, 1);
        // (dynamic_cast<InternalMapState<int, std::string, int, int>*>(&map_state_2))->put(1, 1);
        // state = (dynamic_cast<InternalMapState<int, std::string, int, int>*>(&map_state_2))->get(1);
        // TS_ASSERT_EQUALS(state, 1);

        map_state_2.put(1, 1);
        map_state_2.put(2, 2);

        state = map_state_2.get(1);
        TS_ASSERT_EQUALS(state, 1);
        state = map_state_2.get(2);
        TS_ASSERT_EQUALS(state, 2);

        map_state_2.put(1, 10);
        state = map_state_2.get(1);
        TS_ASSERT_EQUALS(state, 10);

        bool is_contain = map_state_2.contains(1);
        TS_ASSERT_EQUALS(is_contain, true);
        internal_map_state_2.set_current_namespace("ns-2");
        is_contain = map_state_2.contains(1);
        TS_ASSERT_EQUALS(is_contain, false);
        internal_map_state_2.set_current_namespace("ns-1");
        is_contain = map_state_2.contains(1);
        TS_ASSERT_EQUALS(is_contain, true);
    }

    void testKeyedMapStateStore( void ) {
        std::cout << "test testKeyedMapStateStore()" << std::endl;
        KeyedMapStateStore* state_store = new TestMapKeyedMapStateStore<int, int>();
        MapStateDescriptor<int, int> map_state_desc_2("map-state2");
        MapState<int, int>* map_state = nullptr;
        state_store->get_map_state(map_state_desc_2, map_state);
    }

    void testHeapMapState( void ) {
        std::cout << "test testHeapMapState()" << std::endl;
        InternalKeyContext<int>* key_context = new InternalKeyContextImpl<int>(KeyGroupRange(0, 10), 3);
        NestedMapsStateTable<int, std::string, std::vector<int>> state_table(key_context);

        HeapListState<int, std::string, int> heap_list_state(state_table, std::vector<int>());

        heap_list_state.set_current_namespace("ns-1");
    }
};

