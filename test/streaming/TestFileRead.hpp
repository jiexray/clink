#pragma once
#include "StreamFlatMap.hpp"
#include "cxxtest/TestSuite.h"
#include "ResultPartitionFactory.hpp"
#include "InputGateFactory.hpp"
#include "RuntimeEnvironment.hpp"
#include "OneInputStreamOperator.hpp"
#include "OneInputStreamTask.hpp"
#include "FileReadFunction.hpp"

class MySinkFunction: public SinkFunction<std::string> {
    void invoke(std::string& val) override {
        std::cout << "MySinkFunction::invoke(), get " << val << std::endl;
    }

    char* serialize() override {return (char*)this;}

    std::shared_ptr<SinkFunction<std::string>> deserialize() override { return std::make_shared<MySinkFunction>();}
};

class TestFlatMap: public CxxTest::TestSuite
{
public:
    void testStreamFlatMap() {
        std::cout << "test testStreamFlatMap()" << std::endl;
        std::shared_ptr<StreamOperator<std::string>> stream_flat_map = std::make_shared<StreamFlatMap<std::string, std::string>>(std::make_shared<FileReadFunction>());
        std::shared_ptr<StreamOperatorFactory<std::string>> operator_factory_1 = SimpleStreamOperatorFactory<std::string>::of(stream_flat_map);
        std::shared_ptr<StreamOperator<std::string>> stream_sink = std::make_shared<StreamSink<std::string>>(std::make_shared<MySinkFunction>());
        std::shared_ptr<StreamOperatorFactory<std::string>> operator_factory_2 = SimpleStreamOperatorFactory<std::string>::of(stream_sink);

        std::string node_name_1("flat-map");
        std::string node_name_2("sink");
        std::shared_ptr<StreamNode<std::string>> node_1 = std::make_shared<StreamNode<std::string>>(0, operator_factory_1, node_name_1);
        std::shared_ptr<StreamNode<std::string>> node_2 = std::make_shared<StreamNode<std::string>>(1, operator_factory_2, node_name_2);

        std::shared_ptr<StreamPartitioner<std::string>> stream_partitioner = std::make_shared<ForwardPartitioner<std::string>>();

        std::shared_ptr<StreamEdge<std::string>> edge = std::make_shared<StreamEdge<std::string>>(node_1, node_2, stream_partitioner);


        std::shared_ptr<BufferPool> buffer_pool = std::make_shared<BufferPool>(100, 5);
        std::shared_ptr<ResultPartitionManager> result_partition_manager = std::make_shared<ResultPartitionManager>();
        
        InputGateFactory input_gate_factory(result_partition_manager);

        ResultPartitionFactory result_partition_factory(result_partition_manager);

        /* Create RuntimeEnvironment of flat-map StreamTask*/
        int job_id = 0;
        int job_vertex_id_1 = 0;
        int execution_id_1 = 0;
        int num_of_result_partitions_1 = 1;
        int num_of_input_gates_1 = 1;
        std::shared_ptr<ResultPartition> *result_partitions_1 = new std::shared_ptr<ResultPartition>[num_of_result_partitions_1];
        std::shared_ptr<InputGate> *input_gates_1 = new std::shared_ptr<InputGate>[num_of_input_gates_1];
        for (int i = 0; i < num_of_result_partitions_1; i++) {
            result_partitions_1[i] =  result_partition_factory.create("flat-map-rp", i, std::make_shared<ResultPartitionDeploymentDescriptor>(1), buffer_pool);
        }
        for (int i = 0; i < num_of_input_gates_1; i++) {
            input_gates_1[i] = input_gate_factory.create("flat-map-ig", i, std::make_shared<InputGateDeploymentDescriptor>(0, new std::string{"fake-test-task-0"}, 1));
        }
        /* set configuration */
        std::shared_ptr<Configuration> test_conf_1 = std::make_shared<Configuration>();
        test_conf_1->set_edge<std::string>(StreamConfig::EDGE_NAME, edge);
        test_conf_1->set_operator_factory<std::string, std::string>(StreamConfig::OPERATOR_FACTORY, operator_factory_1);
        test_conf_1->set_value<int>(StreamConfig::NUMBER_OF_INPUTS, std::make_shared<int>(1));

        std::shared_ptr<TaskInfo> task_info_1 = std::make_shared<TaskInfo>("flat-map", "flat-map-0", 0, 1);

        std::shared_ptr<Environment> env_1 = std::make_shared<RuntimeEnvironment>(job_id, job_vertex_id_1, execution_id_1, result_partitions_1, num_of_result_partitions_1,
                                                                                input_gates_1, num_of_input_gates_1, test_conf_1, task_info_1);

        /* Create Result Partition */
        // Result partition should initialize first, and the input gate follows it.

        /* Each ResultPartition has 4 Subpartitions */
        std::shared_ptr<ResultPartition> partition_0 = result_partition_factory.create("fake-test-task", 0, 
                                                                                    std::make_shared<ResultPartitionDeploymentDescriptor>(4), buffer_pool);
        


        /* Create RuntimeEnvironment of sink StreamTask*/
        int job_vertex_id_2 = 1;
        int execution_id_2 = 1;
        int num_of_result_partitions_2 = 0;
        int num_of_input_gates_2 = 1;
        std::shared_ptr<InputGate> *input_gates_2 = new std::shared_ptr<InputGate>[num_of_input_gates_2];
        for (int i = 0; i < num_of_input_gates_2; i++) {
            input_gates_2[i] = input_gate_factory.create("test-input-gate", i, std::make_shared<InputGateDeploymentDescriptor>(0, new std::string{"flat-map-rp-0"}, 1));
        }
        /* set configuration */
        std::shared_ptr<Configuration> test_conf_2 = std::make_shared<Configuration>();
        test_conf_2->set_edge<std::string>(StreamConfig::EDGE_NAME, nullptr);
        test_conf_2->set_operator_factory<std::string, std::string>(StreamConfig::OPERATOR_FACTORY, operator_factory_2);
        test_conf_2->set_value<int>(StreamConfig::NUMBER_OF_INPUTS, std::make_shared<int>(1));

        std::shared_ptr<TaskInfo> task_info_2 = std::make_shared<TaskInfo>("sink", "sink-0", 0, 1);

        std::shared_ptr<Environment> env_2 = std::make_shared<RuntimeEnvironment>(job_id, job_vertex_id_2, execution_id_2, nullptr, num_of_result_partitions_2,
                                                                                input_gates_2, num_of_input_gates_2, test_conf_2, task_info_2);




        std::shared_ptr<StreamTask<std::string>> flat_map_task = std::make_shared<OneInputStreamTask<std::string, std::string>>(env_1);
        flat_map_task->before_invoke();
        std::shared_ptr<StreamTask<std::string>> sink_task = std::make_shared<OneInputStreamTask<std::string, std::string>>(env_2);
        sink_task->before_invoke();


        std::shared_ptr<ResultWriter<std::string>> result_writer_0 = std::make_shared<ResultWriter<std::string>>(partition_0, "test");
        std::shared_ptr<StreamRecord<std::string>> record_1 = std::make_shared<StreamRecord<std::string>>("resource/wordcount.txt");


        result_writer_0->emit(record_1, 0);
        // result_writer_0->flush(0);

        flat_map_task->process_input();
        flat_map_task->process_input();
        sink_task->process_input();
        sink_task->process_input();
        sink_task->process_input();
        sink_task->process_input();
        sink_task->process_input();
        sink_task->process_input();
        sink_task->process_input();
        sink_task->process_input();
        sink_task->process_input();
        sink_task->process_input();
        sink_task->process_input();
    }
};

