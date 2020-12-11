#pragma once

#include "cxxtest/TestSuite.h"
#include "WindowOperator.hpp"
#include "ProcessAllWindowFunction.hpp"
#include "TimeWindow.hpp"
#include "TumblingProcessingTimeWindows.hpp"
#include "TumblingEventTimeWindows.hpp"
#include "ProcessingTimeTrigger.hpp"
#include "EventTimeTrigger.hpp"
#include "InternalKeyContextImpl.hpp"
#include "ProcessingTimeServiceImpl.hpp"
#include "InternalSingleValueProcessAllWindowFunction.hpp"
#include "InternalIterableProcessAllWindowFunction.hpp"
#include "HeapListState.hpp"
#include "AggregateFunction.hpp"
#include "WindowOperatorFactory.hpp"
#include "AggregatingStateDescriptor.hpp"
#include "MemoryStateBackend.hpp"
#include "HeapAggregatingState.hpp"
#include "LoggerFactory.hpp"

#include <iostream>
#include <memory>
#include <chrono>
#include <thread>

class TestWindowFunction: public ProcessAllWindowFunction<int, int, TimeWindow>{
public:
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

    static ProcessAllWindowFunction<int, int, TimeWindow>* create() {
        return new TestWindowFunction();
    }
};

class TestAggregateFunction: public AggregateFunction<int, int, int> {
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

    static AggregateFunction<int, int, int>* create() {
        return new TestAggregateFunction();
    }
};

class TestWindowOperatorWithEventTime: public CxxTest::TestSuite
{
    typedef StateBackend<int, TimeWindow, int, AppendingState<int, int>, InternalAppendingState<int, TimeWindow, int, int, int>> StateBackendType;
    typedef MemoryStateBackend<int, TimeWindow, int, AppendingState<int, int>, InternalAppendingState<int, TimeWindow, int, int, int>> MemoryStateBackendType;
    
public:
    void createLoggerFactory() {
        std::shared_ptr<spdlog::logger> logger = LoggerFactory::get_logger("TestWindowOperatorFactory");
    }

    void testCreator( void ) {
        typedef std::function<WindowAssigner<int, TimeWindow>*(void)> WindowAssignerCreator;
        WindowAssignerCreator window_assinger_creator = std::bind(TumblingProcessingTimeWindows<int>::create, 1000l, 0l);
        WindowAssigner<int, TimeWindow>* window_assigner = window_assinger_creator();
        // std::cout << "WindowAssigner: " << window_assigner->to_string() << std::endl;
    } 

    void testFactoryCreateOperator( void ) {
        // std::cout << "test testFactoryCreateOperator()" << std::endl;

        TestWindowFunction window_func;
        TestAggregateFunction agg_function;

        WindowOperatorFactory<int, int, int, int> window_operator_factory(
            std::bind(TumblingEventTimeWindows<int>::create, 1000l, 0l),
            std::bind(InternalSingleValueProcessAllWindowFunction<int, int, int, TimeWindow>::create, TestWindowFunction::create),
            EventTimeTrigger<int>::create,
            std::bind(AggregatingStateDescriptor<int, int, int>::create, "test-operator-factory", TestAggregateFunction::create, 0),
            HeapAggregatingState<int, TimeWindow, int, int, int>::create_appending<InternalAppendingState<int, TimeWindow, int, int, int>>);

        MemoryStateBackendType state_backend;

        window_operator_factory.set_state_backend(state_backend);

        std::shared_ptr<Output<int>> output = nullptr;

        SystemProcessingTimeService system_time_service;
        ProcessingTimeServiceImpl time_service(system_time_service);

        ExecutionConfig execution_config;

        std::shared_ptr<TaskInfo> task_info = std::make_shared<TaskInfo>("window-operator-factory-test", 0, 1);

        std::shared_ptr<StreamOperatorParameters<int>> parameters = std::make_shared<StreamOperatorParameters<int>>(
                output,
                time_service,
                execution_config,
                task_info);
        std::shared_ptr<StreamOperator<int>> window_operator = window_operator_factory.create_stream_operator(parameters);

        std::dynamic_pointer_cast<AbstractUdfStreamOperator<Function, int>>(window_operator)->open();

        for (int i = 0; i < 1000; i++) {
            StreamRecordV2<int> int_record_1(1, TimeUtil::current_timestamp());
            std::dynamic_pointer_cast<OneInputStreamOperator<int, int>>(window_operator)->process_element(&int_record_1);

            if (i % 10 == 0) {
                StreamRecordV2<int> watermark(TimeUtil::current_timestamp());
                std::dynamic_pointer_cast<OneInputStreamOperator<int, int>>(window_operator)->process_watermark(&watermark);
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        StreamRecordV2<int> watermark(TimeUtil::current_timestamp());
        std::dynamic_pointer_cast<OneInputStreamOperator<int, int>>(window_operator)->process_watermark(&watermark);

        std::this_thread::sleep_for(std::chrono::seconds(10));
    }
};
