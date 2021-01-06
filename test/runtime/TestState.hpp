#pragma once

#include "cxxtest/TestSuite.h"
#include "NestedStateMap.hpp"
#include "NestedMapsStateTable.hpp"
#include "InternalKeyContextImpl.hpp"
#include "HeapMapState.hpp"
#include "KeyGroupRangeAssignment.hpp"
#include "HeapKeyedStateBackend.hpp"
#include "MapStateDescriptor.hpp"
#include "KeyedMapStateStore.hpp"
#include "HeapListState.hpp"
#include "ListStateDescriptor.hpp"
#include "TimeUtil.hpp"
#include "InternalTimerServiceImpl.hpp"
#include "KeyContext.hpp"
#include "ProcessingTimeServiceImpl.hpp"
#include "SystemProcessingTimeService.hpp"
#include "Triggerable.hpp"
#include "AggregateFunction.hpp"
#include "HeapAggregatingState.hpp"
#include "AggregatingStateDescriptor.hpp"
#include "HeapPriorityQueueSet.hpp"
#include "InternalTimer.hpp"
#include "TimeWindow.hpp"
#include <iostream>
#include <string>
#include <map>
#include <typeinfo>
#include <chrono>
#include <thread>

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


class TestAggregateFunction: public AggregateFunction<int, int, int> {
public:
    int create_accumulator() {
        return 0;
    }

    int add(const int* value, const int* accumulator) {
        if (value == nullptr || accumulator == nullptr) {
            throw std::runtime_error("value or accumulate is nullptr");
        }
        return *value + *accumulator;
    }

    int get_result(const int* accumulator) {
        return *accumulator;
    }

    static AggregateFunction<int, int, int>* create() {
        return new TestAggregateFunction();
    }
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
        NestedMapsStateTable<int, std::string, int> state_table(*key_context);

        key_context->set_current_key(101);

        state_table.put("ns-1", 101);
        state_table.put("ns-1", 102);

        int state_1 = state_table.get("ns-1");
        TS_ASSERT_EQUALS(state_1, 102);
    }

    void testHeapState() {
        std::cout << "test testHeapState()" << std::endl;
        InternalKeyContext<int>* key_context = new InternalKeyContextImpl<int>(KeyGroupRange(0, 10), 3);
        NestedMapsStateTable<int, std::string, std::map<int, int>> state_table(*key_context);

        HeapMapState<int, std::string, int, int> heap_map_state(state_table, std::map<int, int>());

        heap_map_state.set_current_namespace("ns-1");

        heap_map_state.put(10, 1);
        heap_map_state.put(12, 1);
        
        int state = heap_map_state.get(10);
        TS_ASSERT_EQUALS(state, 1);

        heap_map_state.set_current_namespace("ns-2");
        heap_map_state.put(10, 10);
        heap_map_state.put(12, 12);

        state = heap_map_state.get(10);
        TS_ASSERT_EQUALS(state, 10);

        heap_map_state.set_current_namespace("ns-1");
        state = heap_map_state.get(10);
        TS_ASSERT_EQUALS(state, 1);
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

        TaskKvStateRegistry<int, std::string, std::map<int, int>> task_kv_state_registry(kv_state_registry, job_id, job_vertex_id);
        ExecutionConfig execution_config;
        InternalKeyContext<int>* key_context = new InternalKeyContextImpl<int>(KeyGroupRange(0, 10), 3);

        HeapKeyedStateBackend<int, std::string, std::map<int, int>, MapState<int, int>, InternalMapState<int, std::string, int, int>> heap_stated_backend(
                execution_config,
                key_context,
                std::map<std::string, StateTable<int, std::string, std::map<int, int>>*>());
    
        MapStateDescriptor<int, int> map_state_desc("map-state");

        heap_stated_backend.register_state_creator(
            map_state_desc.get_state_descriptor_id(),
            HeapMapState<int, std::string, int, int>::create<InternalMapState<int, std::string, int, int>>);

        InternalMapState<int, std::string, int, int>* map_state = heap_stated_backend.create_internal_state(map_state_desc);
        // HeapMapState<int, std::string, int, int>* map_state = dynamic_cast<HeapMapState<int, std::string, int, int>*>(heap_stated_backend.create_internal_state(map_state_desc));
        TS_ASSERT_EQUALS(map_state == nullptr, false);
        map_state->set_current_namespace("ns-1");

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
        // HeapMapState<int, std::string, int, int>& map_state_2 = *(dynamic_cast<HeapMapState<int, std::string, int, int>*>(&heap_stated_backend.get_or_create_keyed_state(map_state_desc_2)));
        // HeapMapState<int, std::string, int, int>& map_state_2 = (HeapMapState<int, std::string, int, int>&)heap_stated_backend.get_or_create_keyed_state(map_state_desc_2);
        TS_ASSERT_EQUALS(&map_state_2 == nullptr, false);
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

        bool is_contain = map_state_2.contains(1);
        TS_ASSERT_EQUALS(is_contain, true);
        internal_map_state_2.set_current_namespace("ns-2");
        is_contain = map_state_2.contains(1);
        TS_ASSERT_EQUALS(is_contain, false);
        map_state_2.put(1, 10);
        state = map_state_2.get(1);
        TS_ASSERT_EQUALS(state, 10);

        internal_map_state_2.set_current_namespace("ns-1");
        state = map_state_2.get(1);
        TS_ASSERT_EQUALS(state, 1);
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
        NestedMapsStateTable<int, std::string, std::vector<int>> state_table(*key_context);

        HeapListState<int, std::string, int> heap_list_state(state_table, std::vector<int>());

        heap_list_state.set_current_namespace("ns-1");

        heap_list_state.add(10);
        heap_list_state.add(11);
        const std::vector<int>& cur_vec = heap_list_state.get();
        TS_ASSERT_EQUALS(cur_vec.size(), 2);
        TS_ASSERT_EQUALS(cur_vec[0], 10);
        TS_ASSERT_EQUALS(cur_vec[1], 11);

        heap_list_state.add_all(std::vector<int>{12, 13, 14});
        TS_ASSERT_EQUALS(cur_vec.size(), 5);
        TS_ASSERT_EQUALS(cur_vec[2], 12);
        TS_ASSERT_EQUALS(cur_vec[3], 13);
        TS_ASSERT_EQUALS(cur_vec[4], 14);

        heap_list_state.update(std::vector<int>{101, 102});
        const std::vector<int>& cur_vec_2 = heap_list_state.get();
        TS_ASSERT_EQUALS(cur_vec_2.size(), 2);
        TS_ASSERT_EQUALS(cur_vec_2[0], 101);
        TS_ASSERT_EQUALS(cur_vec_2[1], 102);
    }

    void testHeapStateBackendWithListState( void ){
        std::cout << "test testHeapStateBackendWithListState()" << std::endl;

        KvStateRegistry<int, std::string, std::vector<int>> kv_state_registry;
        int job_id = 0;
        int job_vertex_id = 0;

        TaskKvStateRegistry<int, std::string, std::vector<int>> task_kv_state_registry(kv_state_registry, job_id, job_vertex_id);
        ExecutionConfig execution_config;
        InternalKeyContext<int>* key_context = new InternalKeyContextImpl<int>(KeyGroupRange(0, 10), 3);

        HeapKeyedStateBackend<int, std::string, std::vector<int>, ListState<int>, InternalListState<int, std::string, int>> heap_stated_backend(
                execution_config,
                key_context,
                std::map<std::string, StateTable<int, std::string, std::vector<int>>*>());
    
        heap_stated_backend.set_current_key(101);

        ListStateDescriptor<int> list_state_desc("list-state");

        heap_stated_backend.register_state_creator(
            list_state_desc.get_state_descriptor_id(),
            HeapListState<int, std::string, int>::create<InternalListState<int, std::string, int>>);

        InternalListState<int, std::string, int>* list_state = heap_stated_backend.create_internal_state(list_state_desc);
        list_state->set_current_namespace("ns-1");

        list_state->add(1);
        list_state->add(2);

        const std::vector<int>& cur_vec = list_state->get();
        TS_ASSERT_EQUALS(cur_vec.size(), 2);
        TS_ASSERT_EQUALS(cur_vec[0], 1);
        TS_ASSERT_EQUALS(cur_vec[1], 2);

        list_state->set_current_namespace("ns-2");
        TS_ASSERT_EQUALS(list_state->contains_list(), false);
        list_state->add(3);
        list_state->add(4);

        const std::vector<int>& cur_vec_2 = list_state->get();
        TS_ASSERT_EQUALS(cur_vec_2.size(), 2);
        TS_ASSERT_EQUALS(cur_vec_2[0], 3);
        TS_ASSERT_EQUALS(cur_vec_2[1], 4);

        list_state->set_current_namespace("ns-1");
        const std::vector<int>& cur_vec_3 = list_state->get();
        TS_ASSERT_EQUALS(cur_vec_3.size(), 2);
        TS_ASSERT_EQUALS(cur_vec_3[0], 1);
        TS_ASSERT_EQUALS(cur_vec_3[1], 2);
    }

    void testStateDtor() {
        std::cout << "test testStateDtor()" << std::endl;
        InternalKeyContext<int>* key_context = new InternalKeyContextImpl<int>(KeyGroupRange(0, 10), 3);
        NestedMapsStateTable<int, std::string, std::map<int, int>> state_table(*key_context);

        std::cout << "Create HeapMapState" << std::endl;
        HeapMapState<int, std::string, int, int>* heap_map_state = new HeapMapState<int, std::string, int, int>(state_table, std::map<int, int>());
        delete heap_map_state;

        std::cout << "Create InternalMapState" << std::endl;
        InternalMapState<int, std::string, int, int>* internal_map_state = new HeapMapState<int, std::string, int, int>(state_table, std::map<int, int>());
        delete internal_map_state;

        std::cout << "Create MapState" << std::endl;
        MapState<int, int>* map_state = new HeapMapState<int, std::string, int, int>(state_table, std::map<int, int>());
        delete map_state;
    }

    void testCurrentTimestamp() {
        std::cout << "test testCurrentTimestamp()" << std::endl;
        std::cout << "current timestamp: " << TimeUtil::current_timestamp() << std::endl;
    }


    // void testInternalTimerService( void ) {
    //     std::cout << "test testInternalTimerService()" << std::endl;

    //     TestKeyContext<int> key_context(10);
    //     SystemProcessingTimeService system_time_service;
    //     ProcessingTimeServiceImpl time_service(system_time_service);

    //     TestTriggerable<int, std::string> triggerable;
    //     InternalTimerServiceImpl<int, std::string> timer_service(
    //         KeyGroupRange(0, 10),
    //         key_context,
    //         time_service,
    //         triggerable);

    //     std::cout << "current timestamp: " << timer_service.current_processing_time() << std::endl;
    //     timer_service.register_processing_time_timer("ns-1", timer_service.current_processing_time() + 500l);
    //     timer_service.register_processing_time_timer("ns-2", timer_service.current_processing_time() + 500l);

    //     std::this_thread::sleep_for(std::chrono::seconds(2));
    // }

    void testAggregate( void ) {
        std::cout << "test testAggregate()" << std::endl;

        TestAggregateFunction agg_function;

        InternalKeyContext<int>* key_context = new InternalKeyContextImpl<int>(KeyGroupRange(0, 10), 3);
        NestedMapsStateTable<int, std::string, int> state_table(*key_context);

        HeapAggregatingState<int, std::string, int, int, int> heap_aggregating_state(state_table, 0, agg_function);

        heap_aggregating_state.set_current_namespace("ns-1");

        heap_aggregating_state.add(10);
        int state = heap_aggregating_state.get();
        TS_ASSERT_EQUALS(state, 10);

        heap_aggregating_state.add(11);
        state = heap_aggregating_state.get();
        TS_ASSERT_EQUALS(state, 21);

        KvStateRegistry<int, std::string, int> kv_state_registry;
        int job_id = 0;
        int job_vertex_id = 0;

        TaskKvStateRegistry<int, std::string, int> task_kv_state_registry(kv_state_registry, job_id, job_vertex_id);
        ExecutionConfig execution_config;

        HeapKeyedStateBackend<int, std::string, int, AggregatingState<int, int>, InternalAggregatingState<int, std::string, int, int, int>> heap_state_backend(
                execution_config,
                key_context,
                std::map<std::string, StateTable<int, std::string, int>*>());
    
        heap_state_backend.set_current_key(101);

        AggregatingStateDescriptor<int, int, int> aggregating_state_desc("aggregating-state", TestAggregateFunction::create(), 1);

        heap_state_backend.register_state_creator(
            aggregating_state_desc.get_state_descriptor_id(),
            HeapAggregatingState<int, std::string, int, int, int>::create<InternalAggregatingState<int, std::string, int, int, int>>);
        
        InternalAggregatingState<int, std::string, int, int, int>* aggregate_state = heap_state_backend.create_internal_state(aggregating_state_desc);
        aggregate_state->set_current_namespace("ns-1");

        aggregate_state->add(100);
        state = aggregate_state->get();
        TS_ASSERT_EQUALS(state, 100);

        aggregate_state->add(101);
        state = aggregate_state->get();
        TS_ASSERT_EQUALS(state, 201);
    }

    void testAppendingState2(void) {
        std::cout << "test testAppendingState2()" << std::endl;
        KvStateRegistry<int, std::string, std::vector<int>> kv_state_registry;
        int job_id = 0;
        int job_vertex_id = 0;

        TaskKvStateRegistry<int, std::string, std::vector<int>> task_kv_state_registry(kv_state_registry, job_id, job_vertex_id);
        ExecutionConfig execution_config;
        InternalKeyContext<int>* key_context = new InternalKeyContextImpl<int>(KeyGroupRange(0, 10), 3);

        HeapKeyedStateBackend<int, std::string, std::vector<int>, AppendingState<int, std::vector<int>>, InternalAppendingState<int, std::string, int, std::vector<int>, std::vector<int>>> heap_stated_backend(
                execution_config,
                key_context,
                std::map<std::string, StateTable<int, std::string, std::vector<int>>*>());
    
        heap_stated_backend.set_current_key(101);

        ListStateDescriptor<int> list_state_desc("list-state");

        heap_stated_backend.register_state_creator(
            list_state_desc.get_state_descriptor_id(),
            HeapListState<int, std::string, int>::create_appending<InternalListState<int, std::string, int>>);
    
        InternalAppendingState<int, std::string, int, std::vector<int>, std::vector<int>>* list_state = heap_stated_backend.create_internal_state(list_state_desc);
        list_state->set_current_namespace("ns-1");

        list_state->add_internal(1);
        list_state->add_internal(2);

        const std::vector<int>& cur_vec = list_state->get_internal();
        TS_ASSERT_EQUALS(cur_vec.size(), 2);
        TS_ASSERT_EQUALS(cur_vec[0], 1);
        TS_ASSERT_EQUALS(cur_vec[1], 2);
    }

    void testAppendingState( void ) {
        std::cout << "test testAppendingState()" << std::endl;
        TestAggregateFunction agg_function;

        KvStateRegistry<int, std::string, int> kv_state_registry;
        int job_id = 0;
        int job_vertex_id = 0;

        TaskKvStateRegistry<int, std::string, int> task_kv_state_registry(kv_state_registry, job_id, job_vertex_id);
        ExecutionConfig execution_config;
        InternalKeyContext<int>* key_context = new InternalKeyContextImpl<int>(KeyGroupRange(0, 10), 3);

        HeapKeyedStateBackend<int, std::string, int, AggregatingState<int, int>, InternalAggregatingState<int, std::string, int, int, int>> heap_state_backend(
                execution_config,
                key_context,
                std::map<std::string, StateTable<int, std::string, int>*>());

        heap_state_backend.set_current_key(101);

        AggregatingStateDescriptor<int, int, int> aggregating_state_desc("aggregating-state", TestAggregateFunction::create(), 1);

        heap_state_backend.register_state_creator(
            aggregating_state_desc.get_state_descriptor_id(),
            HeapAggregatingState<int, std::string, int, int, int>::create<InternalAggregatingState<int, std::string, int, int, int>>);
        

        InternalAppendingState<int, std::string, int, int, int>* appending_state = heap_state_backend.create_internal_state(aggregating_state_desc);
        appending_state->set_current_namespace("ns-1");

        appending_state->update_internal(100);
        int state = appending_state->get_internal();

        TS_ASSERT_EQUALS(state, 100);

        appending_state->update_internal(101);
        state = appending_state->get_internal();
        TS_ASSERT_EQUALS(state, 101);

        appending_state->set_current_namespace("ns-2");
    }

    void testHeapPriorityQueueSet( void ) {
        std::cout << "test testHeapPriorityQueueSet()" << std::endl;

        HeapPriorityQueueSet<TimerHeapInternalTimer<int, TimeWindow>> pq_set(10, KeyGroupRange(0, 10), 20);

        TS_ASSERT_EQUALS(pq_set.empty(), true);

        bool is_heap_changed = pq_set.add(TimerHeapInternalTimer<int, TimeWindow>(10000, 10, TimeWindow(1000, 2000)));
        TS_ASSERT_EQUALS(is_heap_changed, true);
        is_heap_changed = pq_set.add(TimerHeapInternalTimer<int, TimeWindow>(10000, 10, TimeWindow(1000, 2000)));
        TS_ASSERT_EQUALS(is_heap_changed, false);

        const TimerHeapInternalTimer<int, TimeWindow>& head_element = pq_set.top();
        TS_ASSERT_EQUALS(head_element.get_key(), 10);
        TS_ASSERT_EQUALS(head_element.get_timestamp(), 10000);
        TS_ASSERT_EQUALS(head_element.get_namespace().get_start(), 1000);
        TS_ASSERT_EQUALS(head_element.get_namespace().get_end(), 2000);

        is_heap_changed = pq_set.add(TimerHeapInternalTimer<int, TimeWindow>(10001, 10, TimeWindow(1000, 2000)));
        TS_ASSERT_EQUALS(is_heap_changed, false);

        const TimerHeapInternalTimer<int, TimeWindow>& head_element_2 = pq_set.top();
        TS_ASSERT_EQUALS(head_element_2.get_key(), 10);
        TS_ASSERT_EQUALS(head_element_2.get_timestamp(), 10000);
        TS_ASSERT_EQUALS(head_element_2.get_namespace().get_start(), 1000);
        TS_ASSERT_EQUALS(head_element_2.get_namespace().get_end(), 2000);

        is_heap_changed = pq_set.add(TimerHeapInternalTimer<int, TimeWindow>(9999, 10, TimeWindow(999, 1999)));
        TS_ASSERT_EQUALS(is_heap_changed, true);

        const TimerHeapInternalTimer<int, TimeWindow>& head_element_3 = pq_set.top();
        TS_ASSERT_EQUALS(head_element_3.get_key(), 10);
        TS_ASSERT_EQUALS(head_element_3.get_timestamp(), 9999);
        TS_ASSERT_EQUALS(head_element_3.get_namespace().get_start(), 999);
        TS_ASSERT_EQUALS(head_element_3.get_namespace().get_end(), 1999);

        pq_set.pop();
        const TimerHeapInternalTimer<int, TimeWindow>& head_element_4 = pq_set.top();
        TS_ASSERT_EQUALS(head_element_4.get_key(), 10);
        TS_ASSERT_EQUALS(head_element_4.get_timestamp(), 10000);
        TS_ASSERT_EQUALS(head_element_4.get_namespace().get_start(), 1000);
        TS_ASSERT_EQUALS(head_element_4.get_namespace().get_end(), 2000);
    } 
};

