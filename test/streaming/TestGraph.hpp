#pragma once
#include "cxxtest/TestSuite.h"
#include <memory>

#include "../../streaming/graph/StreamEdge.hpp"
#include "../../streaming/graph/StreamNode.hpp"
#include "../../streaming/operators/AbstractUdfStreamOperator.hpp"
#include "../../streaming/operators/StreamMap.hpp"
#include "../../streaming/operators/SimpleStreamOperatorFactory.hpp"
#include "../../streaming/partitioner/ForwardPartitioner.hpp"
#include "../../streaming/task/StreamTask.hpp"
#include "../../streaming/task/OneInputStreamTask.hpp"
#include "../../runtime/RuntimeEnvironment.hpp"
#include "../../result/ResultPartitionFactory.hpp"
#include "../../result/consumer/InputGateFactory.hpp"
#include "../../core/config/Configuration.hpp"


template class MapFunction<std::string, std::string>;
// concat the first char and the last char
class StringMapFunction: public MapFunction<std::string, std::string>{
    std::shared_ptr<std::string> map(std::string& value){
        return std::make_shared<std::string>(value.begin(), value.begin() + 2);
    }

    char* serialize() override {return (char*)this;}

    std::shared_ptr<MapFunction<std::string, std::string>> deserialize() override { return std::make_shared<StringMapFunction>();}
};

class TestGraph: public CxxTest::TestSuite{
public:
    void testNodeAndGraphCreate(void) {
        std::shared_ptr<AbstractUdfStreamOperator<Function, std::string>> stream_map_1 = std::make_shared<StreamMap<std::string, std::string>>(std::make_shared<StringMapFunction>());
        std::shared_ptr<StreamOperatorFactory<std::string>> operator_factory_1 = SimpleStreamOperatorFactory<std::string>::of(stream_map_1);
        std::shared_ptr<AbstractUdfStreamOperator<Function, std::string>> stream_map_2 = std::make_shared<StreamMap<std::string, std::string>>(std::make_shared<StringMapFunction>());
        std::shared_ptr<StreamOperatorFactory<std::string>> operator_factory_2 = SimpleStreamOperatorFactory<std::string>::of(stream_map_2);

        std::string node_name("string-map");
        std::shared_ptr<StreamNode<std::string>> src_node = std::make_shared<StreamNode<std::string>>(0, operator_factory_1, node_name);
        std::shared_ptr<StreamNode<std::string>> target_node = std::make_shared<StreamNode<std::string>>(1, operator_factory_2, node_name);

        std::shared_ptr<StreamPartitioner<std::string>> stream_partitioner = std::make_shared<ForwardPartitioner<std::string>>();

        std::shared_ptr<StreamEdge<std::string>> edge = std::make_shared<StreamEdge<std::string>>(src_node, target_node, stream_partitioner);
    }

    void testConfigurationAddEdge( void ) {
        std::shared_ptr<AbstractUdfStreamOperator<Function, std::string>> stream_map_1 = std::make_shared<StreamMap<std::string, std::string>>(std::make_shared<StringMapFunction>());
        std::shared_ptr<StreamOperatorFactory<std::string>> operator_factory_1 = SimpleStreamOperatorFactory<std::string>::of(stream_map_1);
        std::shared_ptr<AbstractUdfStreamOperator<Function, std::string>> stream_map_2 = std::make_shared<StreamMap<std::string, std::string>>(std::make_shared<StringMapFunction>());
        std::shared_ptr<StreamOperatorFactory<std::string>> operator_factory_2 = SimpleStreamOperatorFactory<std::string>::of(stream_map_2);

        std::string node_name("string-map");
        std::shared_ptr<StreamNode<std::string>> src_node = std::make_shared<StreamNode<std::string>>(0, operator_factory_1, node_name);
        std::shared_ptr<StreamNode<std::string>> target_node = std::make_shared<StreamNode<std::string>>(1, operator_factory_2, node_name);

        std::shared_ptr<StreamPartitioner<std::string>> stream_partitioner = std::make_shared<ForwardPartitioner<std::string>>();

        std::shared_ptr<StreamEdge<std::string>> edge = std::make_shared<StreamEdge<std::string>>(src_node, target_node, stream_partitioner);

        std::shared_ptr<Configuration> conf = std::make_shared<Configuration>();

        conf->set_edge<std::string>("test_edge", edge);

        std::shared_ptr<StreamEdge<std::string>> gen_edge = conf->get_edge<std::string>("test_edge");

        TS_ASSERT_EQUALS(edge->get_source_id(), gen_edge->get_source_id());
        TS_ASSERT_EQUALS(edge->get_target_id(), gen_edge->get_target_id());
        TS_ASSERT_EQUALS(edge->get_source_operator_name(), gen_edge->get_source_operator_name());
        TS_ASSERT_EQUALS(edge->get_target_operator_name(), gen_edge->get_target_operator_name());
        TS_ASSERT_EQUALS(edge->get_output_partitioner()->to_string(), gen_edge->get_output_partitioner()->to_string());
    }

    void testConfigurationAddInt( void ) {
        std::shared_ptr<Configuration> conf = std::make_shared<Configuration>();
        conf->set_value<int>("test_int", std::make_shared<int>(10));
        std::shared_ptr<int> gen_int = conf->get_value<int>("test_int");
        TS_ASSERT_EQUALS(*(gen_int.get()), 10);
    }

    void testConfigurationAddOperator( void ) {
        std::shared_ptr<StreamRecord<std::string>> record_1 = std::make_shared<StreamRecord<std::string>>("12345");
        std::shared_ptr<OneInputStreamOperator<std::string, std::string>> stream_map_1 = 
                        std::make_shared<StreamMap<std::string, std::string>>(std::make_shared<StringMapFunction>());
        std::shared_ptr<Configuration> test_conf = std::make_shared<Configuration>();
        test_conf->set_stream_operator<std::string, std::string>("test-operator", stream_map_1);
        std::shared_ptr<OneInputStreamOperator<std::string, std::string>> gen_stream_map = test_conf->get_stream_operator<std::string, std::string>("test-operator");
        TS_ASSERT_EQUALS(gen_stream_map == nullptr, false);
        gen_stream_map->process_element(record_1);
        TS_ASSERT_EQUALS(stream_map_1.get() == gen_stream_map.get(), false);
    }

    void testConfigurationAddOperatorFactory( void ) {
        std::shared_ptr<StreamRecord<std::string>> record_1 = std::make_shared<StreamRecord<std::string>>("12345");
        std::shared_ptr<AbstractUdfStreamOperator<Function, std::string>> stream_map_1 = std::make_shared<StreamMap<std::string, std::string>>(std::make_shared<StringMapFunction>());
        std::shared_ptr<StreamOperatorFactory<std::string>> operator_factory_1 = SimpleStreamOperatorFactory<std::string>::of(stream_map_1);

        std::shared_ptr<Configuration> test_conf = std::make_shared<Configuration>();
        test_conf->set_operator_factory<std::string, std::string>("test-operator-factory", operator_factory_1);
        std::shared_ptr<StreamOperatorFactory<std::string>> gen_operator_factory = test_conf->get_operator_factory<std::string, std::string>("test-operator-factory");
        TS_ASSERT_EQUALS(gen_operator_factory == nullptr, false);

        /* Create operator, operator chain -> head operator */
        std::shared_ptr<StreamTask<std::string>> stream_task = std::make_shared<StreamTask<std::string>>();

        /**
         * Create Output.
         */
        int* partition_idxs = new int[5] {0};
        /* set consumed_subpartition_index to 0 */
        std::shared_ptr<InputGateDeploymentDescriptor> input_gate_deployment_descriptor = std::make_shared<InputGateDeploymentDescriptor>(0, partition_idxs, 1);
        std::shared_ptr<BufferPool> buffer_pool = std::make_shared<BufferPool>(50, 5);
        std::shared_ptr<ResultPartitionManager> result_partition_manager = std::make_shared<ResultPartitionManager>();

        // Result partition should initialize first, and the input gate follows it.
        ResultPartitionFactory result_partition_factory(result_partition_manager);
        /* Each ResultPartition has 4 Subpartitions */
        std::shared_ptr<ResultPartition> partition_0 = result_partition_factory.create("test", 0, 
                                                                                    std::make_shared<ResultPartitionDeploymentDescriptor>(4), buffer_pool);

        std::shared_ptr<ChannelSelector<std::string>> channel_selector = std::make_shared<ForwardPartitioner<std::string>>();
        std::shared_ptr<ChannelSelectorResultWriter<std::string>> channel_selector_result_writer = 
                std::make_shared<ChannelSelectorResultWriter<std::string>>(partition_0, "test", channel_selector);

        std::shared_ptr<OperatorChain<std::string>> op_chain = std::make_shared<OperatorChain<std::string>>(stream_task, channel_selector_result_writer, gen_operator_factory);

        std::shared_ptr<StreamOperator<std::string>> stream_op = op_chain->get_head_operator();

        (std::dynamic_pointer_cast<OneInputStreamOperator<std::string, std::string>>(stream_op))->process_element(record_1); 
    }


    void testStreamTaskInit( void ) {
        std::shared_ptr<AbstractUdfStreamOperator<Function, std::string>> stream_map_1 = std::make_shared<StreamMap<std::string, std::string>>(std::make_shared<StringMapFunction>());
        std::shared_ptr<StreamOperatorFactory<std::string>> operator_factory_1 = SimpleStreamOperatorFactory<std::string>::of(stream_map_1);
        std::shared_ptr<AbstractUdfStreamOperator<Function, std::string>> stream_map_2 = std::make_shared<StreamMap<std::string, std::string>>(std::make_shared<StringMapFunction>());
        std::shared_ptr<StreamOperatorFactory<std::string>> operator_factory_2 = SimpleStreamOperatorFactory<std::string>::of(stream_map_2);

        std::string node_name("string-map");
        std::shared_ptr<StreamNode<std::string>> src_node = std::make_shared<StreamNode<std::string>>(0, operator_factory_1, node_name);
        std::shared_ptr<StreamNode<std::string>> target_node = std::make_shared<StreamNode<std::string>>(1, operator_factory_2, node_name);

        std::shared_ptr<StreamPartitioner<std::string>> stream_partitioner = std::make_shared<ForwardPartitioner<std::string>>();

        std::shared_ptr<StreamEdge<std::string>> edge = std::make_shared<StreamEdge<std::string>>(src_node, target_node, stream_partitioner);


        std::shared_ptr<BufferPool> buffer_pool = std::make_shared<BufferPool>(100, 5);
        std::shared_ptr<ResultPartitionManager> result_partition_manager = std::make_shared<ResultPartitionManager>();

        int* partition_idxs = new int[5] {1};
        
        InputGateFactory input_gate_factory(result_partition_manager);

        ResultPartitionFactory result_partition_factory(result_partition_manager);

        /* Create RuntimeEnvironment */
        int job_id = 0;
        int job_vertex_id = 0;
        int execution_id = 0;
        int num_of_result_partitions = 1;
        int num_of_input_gates = 1;
        std::shared_ptr<ResultPartition> *result_partitions = new std::shared_ptr<ResultPartition>[num_of_result_partitions];
        std::shared_ptr<InputGate> *input_gates = new std::shared_ptr<InputGate>[num_of_input_gates];
        for (int i = 0; i < num_of_result_partitions; i++) {
            result_partitions[i] =  result_partition_factory.create("test-result-partition", i, std::make_shared<ResultPartitionDeploymentDescriptor>(1), buffer_pool);
        }
        for (int i = 0; i < num_of_input_gates; i++) {
            input_gates[i] = input_gate_factory.create("test-input-gate", i, std::make_shared<InputGateDeploymentDescriptor>(0, partition_idxs, 1));
        }
        /* set configuration */
        std::shared_ptr<Configuration> test_conf = std::make_shared<Configuration>();
        test_conf->set_edge<std::string>(StreamConfig::EDGE_NAME, edge);
        test_conf->set_operator_factory<std::string, std::string>(StreamConfig::OPERATOR_FACTORY, operator_factory_1);
        test_conf->set_value<int>(StreamConfig::NUMBER_OF_INPUTS, std::make_shared<int>(1));

        std::shared_ptr<TaskInfo> task_info = std::make_shared<TaskInfo>("test-task", "test-0", 0, 1);

        std::shared_ptr<Environment> env = std::make_shared<RuntimeEnvironment>(job_id, job_vertex_id, execution_id, result_partitions, num_of_result_partitions,
                                                                                input_gates, num_of_input_gates, test_conf, task_info);

        /* Create Result Partition */
        // Result partition should initialize first, and the input gate follows it.

        /* Each ResultPartition has 4 Subpartitions */
        std::shared_ptr<ResultPartition> partition_0 = result_partition_factory.create("test-result-partition-1", 1, 
                                                                                    std::make_shared<ResultPartitionDeploymentDescriptor>(4), buffer_pool);
        
        std::shared_ptr<StreamTask<std::string>> stream_task = std::make_shared<OneInputStreamTask<std::string, std::string>>(env);
        stream_task->before_invoke();

        std::shared_ptr<ResultWriter<std::string>> result_writer_0 = std::make_shared<ResultWriter<std::string>>(partition_0, "test");
        std::shared_ptr<StreamRecord<std::string>> record_1 = std::make_shared<StreamRecord<std::string>>("1234");
        std::shared_ptr<StreamRecord<std::string>> record_2 = std::make_shared<StreamRecord<std::string>>("13456");


        result_writer_0->emit(record_1, 0);
        result_writer_0->emit(record_2, 0);
        result_writer_0->flush(0);

        stream_task->process_input();
        stream_task->process_input();
    }
};

