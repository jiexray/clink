#pragma once
#include "cxxtest/TestSuite.h"

#include "../../streaming/graph/StreamEdge.hpp"
#include "../../streaming/graph/StreamNode.hpp"
#include "../../streaming/operators/AbstractUdfStreamOperator.hpp"
#include "../../streaming/operators/StreamMap.hpp"
#include "../../streaming/operators/StreamSink.hpp"
#include "../../streaming/operators/SimpleStreamOperatorFactory.hpp"
#include "../../streaming/partitioner/ForwardPartitioner.hpp"
#include "../../streaming/task/StreamTask.hpp"
#include "../../streaming/task/OneInputStreamTask.hpp"
#include "../../streaming/task/SourceStreamTask.hpp"
#include "../../runtime/RuntimeEnvironment.hpp"
#include "../../result/ResultPartitionFactory.hpp"
#include "../../result/consumer/InputGateFactory.hpp"
#include "../../core/config/Configuration.hpp"
#include "MailboxProcessor.hpp"

#include <iostream>
#include <memory>

class MySinkFunction: public SinkFunction<std::string> {
    void invoke(std::string& val) override {
        std::cout << "MySinkFunction::invoke(), get " << val << std::endl;
    }

    char* serialize() override {return (char*)this;}

    std::shared_ptr<SinkFunction<std::string>> deserialize() override { return std::make_shared<MySinkFunction>();}
};


class TestStreamingWithBufferRelease : public CxxTest::TestSuite
{
public:
    void testBufferReleaseWithSinkOperator( void ) {
        std::cout << "test testBufferReleaseWithSinkOperator()" << std::endl;
        std::cout << "test testSinkStreamTaskInit()" << std::endl;
        std::shared_ptr<StreamSink<std::string>> sink_operator = std::make_shared<StreamSink<std::string>>(std::make_shared<MySinkFunction>());
        std::shared_ptr<StreamOperatorFactory<std::string>> operator_factory = SimpleStreamOperatorFactory<std::string>::of(sink_operator);

        // sink operator no out-edges

        std::shared_ptr<BufferPool> buffer_pool = std::make_shared<BufferPool>(100, 5);
        std::shared_ptr<ResultPartitionManager> result_partition_manager = std::make_shared<ResultPartitionManager>();
        
        InputGateFactory input_gate_factory(result_partition_manager);

        ResultPartitionFactory result_partition_factory(result_partition_manager);

        /* Create RuntimeEnvironment */
        int job_id = 0;
        int job_vertex_id = 0;
        int execution_id = 0;
        int num_of_result_partitions = 0;
        int num_of_input_gates = 1;
        std::shared_ptr<InputGate> *input_gates = new std::shared_ptr<InputGate>[num_of_input_gates];
       
        for (int i = 0; i < num_of_input_gates; i++) {
            input_gates[i] = input_gate_factory.create("test-input-gate", i, std::make_shared<InputGateDeploymentDescriptor>(0, new std::string{"fake-test-task-0"}, 1));
        }
        /* set configuration */
        std::shared_ptr<Configuration> test_conf = std::make_shared<Configuration>();
        test_conf->set_edge<std::string>(StreamConfig::EDGE_NAME, nullptr);
        test_conf->set_operator_factory<std::string>(StreamConfig::OPERATOR_FACTORY, operator_factory);
        test_conf->set_value<int>(StreamConfig::NUMBER_OF_INPUTS, std::make_shared<int>(1));

        std::shared_ptr<TaskInfo> task_info = std::make_shared<TaskInfo>("test-sink-task", "test-sink-task-0", 0, 1);

        std::shared_ptr<Environment> env = std::make_shared<RuntimeEnvironment>(job_id, job_vertex_id, execution_id, nullptr, num_of_result_partitions,
                                                                                input_gates, num_of_input_gates, test_conf, task_info);

        /* Create Result Partition */
        // Result partition should initialize first, and the input gate follows it.

        /* Each ResultPartition has 4 Subpartitions */
        std::shared_ptr<ResultPartition> partition_0 = result_partition_factory.create("fake-test-task", 0, 
                                                                                    std::make_shared<ResultPartitionDeploymentDescriptor>(4), buffer_pool);
        
        std::shared_ptr<StreamTask<std::string>> stream_task = std::make_shared<OneInputStreamTask<std::string>>(env);
        stream_task->before_invoke();

        std::shared_ptr<ResultWriter<std::string>> result_writer_0 = std::make_shared<ResultWriter<std::string>>(partition_0, "test");
        std::shared_ptr<StreamRecord<std::string>> record_1 = std::make_shared<StreamRecord<std::string>>("1234");
        std::shared_ptr<StreamRecord<std::string>> record_2 = std::make_shared<StreamRecord<std::string>>("13456");


        result_writer_0->emit(record_1, 0);
        stream_task->process_input();

        // result_writer_0->emit(record_2, 0);
        // stream_task->process_input();
    }
};

