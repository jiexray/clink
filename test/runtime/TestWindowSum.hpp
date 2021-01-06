#pragma once

#include "cxxtest/TestSuite.h"
#include "SourceFunction.hpp"
#include "StreamTaskFactory.hpp"
#include "TaskExecutorRunner.hpp"
#include "JobInformation.hpp"
#include "Configuration.hpp"
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
#include "WindowOperatorFactory.hpp"
#include "AggregatingStateDescriptor.hpp"
#include "MemoryStateBackend.hpp"
#include "HeapAggregatingState.hpp"
#include "LoggerFactory.hpp"
#include "Configuration.hpp"
#include "StreamConfig.hpp"
#include "EventTimeTrigger.hpp"
#include "TumblingEventTimeWindows.hpp"
#include "TimeUtil.hpp"


#include <iostream>

class MySourceFunction: public SourceFunction<int> {
private:
    int total_cnt = 100000000;
    int cnt = 0;
public:
    void run (std::shared_ptr<SourceContext<int>> ctx) override {
        for (int i = 0; i < total_cnt; i++) {
            int val = 1;
            ctx->collect_with_timestamp(&val, TimeUtil::current_timestamp());

            cnt++;

            if (cnt % 1000 == 0) {
                ctx->emit_watermark(TimeUtil::current_timestamp() - 50);
            }
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));
        ctx->emit_watermark(TimeUtil::current_timestamp());
    }

    char* serialize() override {return (char*)this;}

    std::shared_ptr<SourceFunction<int>> deserialize() override {return std::make_shared<MySourceFunction>();}
};

class TestWindowFunction: public ProcessAllWindowFunction<int, int, TimeWindow>{
public:
    void process(
            ProcessAllWindowFunctionContext<int, int, TimeWindow>& context, 
            const std::vector<int>& elements, 
            std::shared_ptr<Collector<int>> out) override {
        // std::cout << "TestWindowFunction::process()" << std::endl;
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

class MySinkFunction: public SinkFunction<int> {
    void invoke(int* val) {
        std::cout << "Sink get val: " << *val << std::endl;
    }

    char* serialize() override {
        return (char*)this;
    }

    std::shared_ptr<SinkFunction<int>> deserialize(char* des_ptr) override {
        return std::make_shared<MySinkFunction>();
    }
};


class TestWindowSum: public CxxTest::TestSuite {
    typedef std::shared_ptr<Configuration> ConfigurationPtr;
public:
    void testCalWindowSum( void ) {
        std::cout << "test testCalWindowSum()" << std::endl;

        // Initialize StreamTaskFactory
        StreamTaskFactory<>::instance()->register_stream_task(typeid(SourceStreamTask<int>).name(), StreamTaskFactoryCreator::create_source_stream_task<int>);
        StreamTaskFactory<>::instance()->register_stream_task(typeid(OneInputStreamTask<int, int>).name(), StreamTaskFactoryCreator::create_one_input_stream_task<int, int>);
        StreamTaskFactory<>::instance()->register_stream_task(typeid(OneInputStreamTask<int>).name(), StreamTaskFactoryCreator::create_one_input_stream_task<int>);

        // Create TaskExecutor
        ConfigurationPtr task_executor_configuration = nullptr;
        std::shared_ptr<TaskExecutorRunner> task_executor_runner = std::make_shared<TaskExecutorRunner>(nullptr, "tm-1");
        std::shared_ptr<TaskExecutor> task_executor = task_executor_runner->get_task_executor();

        // allocate slot
        int slot_id_1 = 5;
        int slot_id_2 = 4;
        int allocation_id_1 = 999;
        int allocation_id_2 = 998;
        int job_id = 0;
        task_executor->allocate_slot(slot_id_1, job_id, allocation_id_1);
        task_executor->allocate_slot(slot_id_2, job_id, allocation_id_2);

        // create JobInformation and TaskInformation
        std::shared_ptr<JobInformation> job_information = std::make_shared<JobInformation>(job_id, "window_sum");

        std::shared_ptr<Configuration> task_configuration_1 = std::make_shared<Configuration>();
        std::shared_ptr<Configuration> task_configuration_2 = std::make_shared<Configuration>();
        std::shared_ptr<Configuration> task_configuration_3 = std::make_shared<Configuration>();

        /* init operator factory */
        std::shared_ptr<StreamSource<int>> stream_source = std::make_shared<StreamSource<int>>(std::make_shared<MySourceFunction>());
        std::shared_ptr<StreamOperatorFactory<int>> operator_factory_1 = SimpleStreamOperatorFactory<int>::of(stream_source);

        TestAggregateFunction agg_function;
        std::shared_ptr<StreamOperatorFactory<int>> operator_factory_2 = std::make_shared<WindowOperatorFactory<int, int, int, int>>(
            std::bind(TumblingEventTimeWindows<int>::create, 1000l, 0l),
            std::bind(InternalSingleValueProcessAllWindowFunction<int, int, int, TimeWindow>::create, TestWindowFunction::create),
            EventTimeTrigger<int>::create,
            std::bind(AggregatingStateDescriptor<int, int, int>::create, "test-operator-factory", TestAggregateFunction::create, 0),
            HeapAggregatingState<int, TimeWindow, int, int, int>::create_appending<InternalAppendingState<int, TimeWindow, int, int, int>>);

        std::shared_ptr<StreamOperator<>> stream_sink = 
                std::make_shared<StreamSink<int>>(std::make_shared<MySinkFunction>());
        std::shared_ptr<StreamOperatorFactory<>> operator_factory_3 = SimpleStreamOperatorFactory<>::of(stream_sink);

        task_configuration_1->set_operator_factory<int, int>(StreamConfig::OPERATOR_FACTORY, operator_factory_1);
        task_configuration_2->set_window_operator_factory<int, int, int, int>(StreamConfig::OPERATOR_FACTORY, operator_factory_2);
        task_configuration_3->set_operator_factory<int>(StreamConfig::OPERATOR_FACTORY, operator_factory_3);

        task_configuration_1->set_value<int>(StreamConfig::OPERATOR_IS_WINDOW, std::make_shared<int>(0));
        task_configuration_2->set_value<int>(StreamConfig::OPERATOR_IS_WINDOW, std::make_shared<int>(1));
        task_configuration_3->set_value<int>(StreamConfig::OPERATOR_IS_WINDOW, std::make_shared<int>(0));

        task_configuration_1->set_value<std::string>(StreamConfig::OPERATOR_ID, std::make_shared<std::string>("op-1"));
        task_configuration_2->set_value<std::string>(StreamConfig::OPERATOR_ID, std::make_shared<std::string>("op-2"));
        task_configuration_3->set_value<std::string>(StreamConfig::OPERATOR_ID, std::make_shared<std::string>("op-3"));

        task_configuration_1->set_value<std::string>(StreamConfig::OPERATOR_NAME, std::make_shared<std::string>("source-op"));
        task_configuration_2->set_value<std::string>(StreamConfig::OPERATOR_NAME, std::make_shared<std::string>("window-op"));
        task_configuration_3->set_value<std::string>(StreamConfig::OPERATOR_NAME, std::make_shared<std::string>("sink-op"));

        // init edge
        std::string node_name_1("source");
        std::string node_name_2("window");
        std::string node_name_3("sink");

        std::shared_ptr<StreamNode> node_1 = std::make_shared<StreamNode>(0, node_name_1);
        std::shared_ptr<StreamNode> node_2 = std::make_shared<StreamNode>(1, node_name_2);
        std::shared_ptr<StreamNode> node_3 = std::make_shared<StreamNode>(2, node_name_3);

        /* Create edge_1 (source -> window) */
        std::shared_ptr<StreamEdge<int>> edge_1 = std::make_shared<StreamEdge<int>>(node_1, node_2, std::make_shared<ForwardPartitioner<int>>());

        /* Create edge_2 (window -> sink) */
        std::shared_ptr<StreamEdge<int>> edge_2 = std::make_shared<StreamEdge<int>>(node_2, node_3, std::make_shared<ForwardPartitioner<int>>());

        task_configuration_1->set_edge<int>(StreamConfig::EDGE_NAME, edge_1);
        task_configuration_2->set_edge<int>(StreamConfig::EDGE_NAME, edge_2);
        // sink task no out-edge

        // source task no input
        task_configuration_2->set_value<int>(StreamConfig::NUMBER_OF_INPUTS, std::make_shared<int>(1));
        task_configuration_3->set_value<int>(StreamConfig::NUMBER_OF_INPUTS, std::make_shared<int>(1));

        std::shared_ptr<TaskInformation> task_information_1 = std::make_shared<TaskInformation>(
                0,
                "source",
                1,
                task_configuration_1,
                typeid(SourceStreamTask<int>).name());

        std::shared_ptr<TaskInformation> task_information_2 = std::make_shared<TaskInformation>(
                1,
                "window",
                1,
                task_configuration_2,
                typeid(OneInputStreamTask<int, int>).name());

        std::shared_ptr<TaskInformation> task_information_3 = std::make_shared<TaskInformation>(
                2,
                "sink",
                1,
                task_configuration_3,
                typeid(OneInputStreamTask<int>).name());
        
        /* create ResultPartition & InputGate descriptors */
        std::shared_ptr<ResultPartitionDeploymentDescriptor>* result_partitions_1 = new std::shared_ptr<ResultPartitionDeploymentDescriptor>[1];
        result_partitions_1[0] = std::make_shared<ResultPartitionDeploymentDescriptor>(1);
        std::shared_ptr<ResultPartitionDeploymentDescriptor>* result_partitions_2 = new std::shared_ptr<ResultPartitionDeploymentDescriptor>[1];
        result_partitions_2[0] = std::make_shared<ResultPartitionDeploymentDescriptor>(1);

        std::shared_ptr<InputGateDeploymentDescriptor>* input_gates_2 = new std::shared_ptr<InputGateDeploymentDescriptor>[1];
        input_gates_2[0] = std::make_shared<InputGateDeploymentDescriptor>(0, new std::string{"source (1/1)-0"}, 1);
        std::shared_ptr<InputGateDeploymentDescriptor>* input_gates_3 = new std::shared_ptr<InputGateDeploymentDescriptor>[1];
        input_gates_3[0] = std::make_shared<InputGateDeploymentDescriptor>(0, new std::string{"window (1/1)-0"}, 1);

        /* Create TaskDeploymentDescriptor */
        std::shared_ptr<TaskDeploymentDescriptor> tdd_1 = std::make_shared<TaskDeploymentDescriptor>(
                job_information->get_job_id(),
                job_information,
                task_information_1,
                102, // execution id
                allocation_id_1, 
                0,   // subtask idx
                5,   // target slot number
                1,   // number_of_result_partitions
                0,   // number_of_input_gates
                result_partitions_1, // result_partitions
                nullptr // input_gates
                );
        
        std::shared_ptr<TaskDeploymentDescriptor> tdd_2 = std::make_shared<TaskDeploymentDescriptor>(
                job_information->get_job_id(),
                job_information,
                task_information_2,
                103, // execution id
                allocation_id_1, 
                0,   // subtask idx
                5,   // target slot number
                1,   // number_of_result_partitions
                1,   // number_of_input_gates
                result_partitions_2, // result_partitions
                input_gates_2 // input_gates
                );

        std::shared_ptr<TaskDeploymentDescriptor> tdd_3 = std::make_shared<TaskDeploymentDescriptor>(
                job_information->get_job_id(),
                job_information,
                task_information_3,
                104, // execution id
                allocation_id_2, 
                0,   // subtask idx
                5,   // target slot number
                0,   // number_of_result_partitions
                1,   // number_of_input_gates
                nullptr, // result_partitions
                input_gates_3 // input_gates
                );
        
        /* subtask task */
        task_executor->submit_task(tdd_1);
        task_executor->submit_task(tdd_2);
        task_executor->submit_task(tdd_3);

        task_executor->start_task(102);
        task_executor->start_task(103);
        task_executor->start_task(104);

        std::this_thread::sleep_for(std::chrono::seconds(3600));

        std::cout << "start to finish tasks" << std::endl;
        task_executor->cancel_task(102);
        task_executor->cancel_task(103);
        task_executor->cancel_task(104);
    }
};

