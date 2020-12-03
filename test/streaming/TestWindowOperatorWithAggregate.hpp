#pragma once

#include "cxxtest/TestSuite.h"
#include "WindowOperator.hpp"
#include "ProcessAllWindowFunction.hpp"
#include "TimeWindow.hpp"
#include "TumblingProcessingTimeWindows.hpp"
#include "ProcessingTimeTrigger.hpp"
#include "InternalKeyContextImpl.hpp"
#include "ProcessingTimeServiceImpl.hpp"
#include "InternalSingleValueProcessAllWindowFunction.hpp"
#include "InternalIterableProcessAllWindowFunction.hpp"
#include "HeapListState.hpp"
#include "AggregateFunction.hpp"
#include "HeapAggregatingState.hpp"

#include <iostream>
#include <memory>
#include <chrono>
#include <thread>

class TestWindowFunction: public ProcessAllWindowFunction<int, int, TimeWindow>{
    void process(
            ProcessAllWindowFunctionContext<int, int, TimeWindow>& context, 
            const std::vector<int>& elements, 
            std::shared_ptr<Collector<int>> out) override {
        std::cout << "TestWindowFunction::process()" << std::endl;
    }

    void open() {

    }
    void close() {

    }
};

class TestAggregatFunction: public AggregateFunction<int, int, int> {
public:
    int* create_accumulator() {
        return new int(0);
    }

    int* add(const int* value, const int* accumulator) {
        if (value == nullptr || accumulator == nullptr) {
            throw std::runtime_error("value or accumulate is nullptr");
        }
        return new int(*value + *accumulator);
    }

    int* get_result(const int* accumulator) {
        return new int(*accumulator);
    }
};

class TestWindowOperatorWithAggregate: public CxxTest::TestSuite {
public:
    void testWindowOperatorWithAggFunction (void) {
        std::cout << "test testWindowOperatorWithAggFunction()" << std::endl;

        TumblingProcessingTimeWindows<int> window_assigner(1000);
        TestWindowFunction window_func;
        std::shared_ptr<InternalWindowFunction<int, int, int, TimeWindow>> internal_window_func =  
                std::make_shared<InternalSingleValueProcessAllWindowFunction<int, int, int, TimeWindow>>(window_func);

        ProcessingTimeTrigger<int> trigger;

        TestAggregatFunction agg_function;
        AggregatingStateDescriptor<int, int, int> state_desc("window-operator", agg_function, 0);

        // Create StateBackend
        KvStateRegistry<int, TimeWindow, int> kv_state_registry;
        int job_id = 0;
        int job_vertex_id = 0;

        TaskKvStateRegistry<int, TimeWindow, int> task_kv_state_registry(kv_state_registry, job_id, job_vertex_id);
        ExecutionConfig execution_config;
        InternalKeyContext<int>* internal_key_context = new InternalKeyContextImpl<int>(KeyGroupRange(0, 10), 3);

        HeapKeyedStateBackend<int, TimeWindow, int, AppendingState<int, int>, InternalAppendingState<int, TimeWindow, int, int, int>> heap_state_backend(
                task_kv_state_registry, 
                execution_config,
                *internal_key_context,
                std::map<std::string, StateTable<int, TimeWindow, int>*>());

        heap_state_backend.register_state_creator(
            std::string(typeid(StateDescriptor<AppendingState<int, int>, int>).name()),
            HeapAggregatingState<int, TimeWindow, int, int, int>::create_appending<InternalAppendingState<int, TimeWindow, int, int, int>>);

        heap_state_backend.set_current_key(101);

        SystemProcessingTimeService system_time_service;
        ProcessingTimeServiceImpl time_service(system_time_service);

        WindowOperator<int, int, int, int, TimeWindow>* window_operator = new WindowOperator<int, int, int, int, TimeWindow>(
                window_assigner,
                internal_window_func,
                trigger,
                state_desc,
                heap_state_backend,
                execution_config,
                time_service);

        window_operator->open();

        for (int i = 0; i < 200; i++) {
            StreamRecordV2<int> int_record_1(1, TimeUtil::current_timestamp());
            window_operator->process_element(&int_record_1);

            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }

        // StreamRecordV2<int> int_record_1(1, TimeUtil::current_timestamp());
        // window_operator->process_element(&int_record_1);

        // std::this_thread::sleep_for(std::chrono::milliseconds(10));
        // StreamRecordV2<int> int_record_2(2, TimeUtil::current_timestamp());
        // window_operator->process_element(&int_record_2);

        // std::this_thread::sleep_for(std::chrono::milliseconds(10));
        // StreamRecordV2<int> int_record_3(3, TimeUtil::current_timestamp());
        // window_operator->process_element(&int_record_3);

        std::this_thread::sleep_for(std::chrono::seconds(20));

        delete window_operator;
    }
};
