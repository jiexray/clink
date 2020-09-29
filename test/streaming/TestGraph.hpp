#pragma once
#include "cxxtest/TestSuite.h"
#include <memory>

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
#include "StreamConfig.cpp"
#include "OutputFormatSinkFunction.hpp"

template std::shared_ptr<StreamOperatorFactory<std::string>> StreamConfig::get_stream_operator_factory<Tuple2<std::string, int>, std::string>();


template class MapFunction<std::string, std::string>;
// concat the first char and the last char
class StringMapFunction: public MapFunction<std::string, std::string>{
    std::shared_ptr<std::string> map(std::string& value){
        std::cout << "StringMapFunction::map(): " << value << std::endl;
        return std::make_shared<std::string>(value.begin(), value.begin() + 2);
    }

    char* serialize() override {return (char*)this;}

    std::shared_ptr<MapFunction<std::string, std::string>> deserialize() override { return std::make_shared<StringMapFunction>();}
};

class TupleMapFunction: public MapFunction<Tuple2<std::string, int>, std::string>{
    std::shared_ptr<std::string> map(Tuple2<std::string, int>& value){
        std::cout << "TupleMapFunction::map() " << std::string("f0: " + (*value.f0) + ", f1: " + std::to_string(*value.f1)) << std::endl;
        return std::make_shared<std::string>("f0: " + (*value.f0) + ", f1: " + std::to_string(*value.f1));
    }
    char* serialize() override {return (char*)this;}

    std::shared_ptr<MapFunction<Tuple2<std::string, int>, std::string>> deserialize() override { return std::make_shared<TupleMapFunction>();}
};

class MySourceFunction: public SourceFunction<std::string> {
    void run(std::shared_ptr<SourceContext<std::string>> ctx) {
        for(int i = 0; i < 10; i++) {
            ctx->collect(std::make_shared<std::string>("1" + std::to_string(i) + "-test-data"));
        }
    }

    char* serialize() override {return (char*)this;}

    std::shared_ptr<SourceFunction<std::string>> deserialize() override { return std::make_shared<MySourceFunction>();}
};

class MySinkFunction: public SinkFunction<std::string> {
    void invoke(std::string& val) override {
        std::cout << "MySinkFunction::invoke(), get " << val << std::endl;
    }

    char* serialize() override {return (char*)this;}

    std::shared_ptr<SinkFunction<std::string>> deserialize(char* no_use) override { return std::make_shared<MySinkFunction>();}
};


class TestMailboxDefaultAction : public MailboxDefaultAction {
public:
    void run_default_action() override {
        std::cout << "TestMailboxDefaultAction::run_default_action()" << std::endl;
    }
};


class TestGraph: public CxxTest::TestSuite{
public:
    void testNodeAndGraphCreate(void) {
        std::shared_ptr<AbstractUdfStreamOperator<Function, std::string>> stream_map_1 = std::make_shared<StreamMap<std::string, std::string>>(std::make_shared<StringMapFunction>());
        std::shared_ptr<StreamOperatorFactory<std::string>> operator_factory_1 = SimpleStreamOperatorFactory<std::string>::of(stream_map_1);
        std::shared_ptr<AbstractUdfStreamOperator<Function, std::string>> stream_map_2 = std::make_shared<StreamMap<std::string, std::string>>(std::make_shared<StringMapFunction>());
        std::shared_ptr<StreamOperatorFactory<std::string>> operator_factory_2 = SimpleStreamOperatorFactory<std::string>::of(stream_map_2);

        std::string node_name("string-map");
        // std::shared_ptr<StreamNode<std::string>> src_node = std::make_shared<StreamNode<std::string>>(0, operator_factory_1, node_name);
        // std::shared_ptr<StreamNode<std::string>> target_node = std::make_shared<StreamNode<std::string>>(1, operator_factory_2, node_name);

        std::shared_ptr<StreamNode> src_node = std::make_shared<StreamNode>(0, node_name);
        std::shared_ptr<StreamNode> target_node = std::make_shared<StreamNode>(1, node_name);

        std::shared_ptr<StreamPartitioner<std::string>> stream_partitioner = std::make_shared<ForwardPartitioner<std::string>>();

        std::shared_ptr<StreamEdge<std::string>> edge = std::make_shared<StreamEdge<std::string>>(src_node, target_node, stream_partitioner);
    }

    void testConfigurationAddEdge( void ) {
        std::shared_ptr<AbstractUdfStreamOperator<Function, std::string>> stream_map_1 = std::make_shared<StreamMap<std::string, std::string>>(std::make_shared<StringMapFunction>());
        std::shared_ptr<StreamOperatorFactory<std::string>> operator_factory_1 = SimpleStreamOperatorFactory<std::string>::of(stream_map_1);
        std::shared_ptr<AbstractUdfStreamOperator<Function, std::string>> stream_map_2 = std::make_shared<StreamMap<std::string, std::string>>(std::make_shared<StringMapFunction>());
        std::shared_ptr<StreamOperatorFactory<std::string>> operator_factory_2 = SimpleStreamOperatorFactory<std::string>::of(stream_map_2);

        std::string node_name("string-map");
        // std::shared_ptr<StreamNode<std::string>> src_node = std::make_shared<StreamNode<std::string>>(0, operator_factory_1, node_name);
        // std::shared_ptr<StreamNode<std::string>> target_node = std::make_shared<StreamNode<std::string>>(1, operator_factory_2, node_name);

        std::shared_ptr<StreamNode> src_node = std::make_shared<StreamNode>(0, node_name);
        std::shared_ptr<StreamNode> target_node = std::make_shared<StreamNode>(1, node_name);

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

    void testConfigurationAddStreamMap( void ) {
        std::shared_ptr<StreamRecord<std::string>> record_1 = std::make_shared<StreamRecord<std::string>>(std::make_shared<std::string>("12345", 5));
        std::shared_ptr<OneInputStreamOperator<std::string, std::string>> stream_map_1 = 
                        std::make_shared<StreamMap<std::string, std::string>>(std::make_shared<StringMapFunction>());
        std::shared_ptr<Configuration> test_conf = std::make_shared<Configuration>();
        test_conf->set_stream_operator<std::string, std::string>("test-operator", stream_map_1);
        std::shared_ptr<OneInputStreamOperator<std::string, std::string>> gen_stream_map = 
            std::dynamic_pointer_cast<OneInputStreamOperator<std::string, std::string>>(test_conf->get_stream_operator<std::string, std::string>("test-operator"));
        TS_ASSERT_EQUALS(gen_stream_map == nullptr, false);
        gen_stream_map->process_element(record_1);
        TS_ASSERT_EQUALS(stream_map_1.get() == gen_stream_map.get(), false);
    }

    void testConfigurationAddStreamSource( void ) {
        std::cout << "test testConfigurationAddStreamSource()" << std::endl;
        std::shared_ptr<StreamSource<std::string>> stream_source = std::make_shared<StreamSource<std::string>>(std::make_shared<MySourceFunction>());
        std::shared_ptr<Configuration> test_conf = std::make_shared<Configuration>();
        std::shared_ptr<StreamOperatorFactory<std::string>> operator_factory = SimpleStreamOperatorFactory<std::string>::of(stream_source);

        test_conf->set_operator_factory<std::string, std::string>(StreamConfig::OPERATOR_FACTORY, operator_factory);
        std::shared_ptr<StreamOperatorFactory<std::string>> gen_operator_factory = test_conf->get_operator_factory<std::string, std::string>(StreamConfig::OPERATOR_FACTORY);
        TS_ASSERT_EQUALS(gen_operator_factory == nullptr, false);

        std::shared_ptr<StreamSource<std::string>> gen_stream_source = 
            std::dynamic_pointer_cast<StreamSource<std::string>>(std::dynamic_pointer_cast<SimpleUdfStreamOperatorFactory<std::string>>(gen_operator_factory)->get_operator());
        
        TS_ASSERT_EQUALS(gen_stream_source == nullptr, false);
        // gen_stream_source->run();
    }

    void testConfigurationAddOperatorFactory( void ) {
        std::shared_ptr<StreamRecord<std::string>> record_1 = std::make_shared<StreamRecord<std::string>>(std::make_shared<std::string>("12345", 5));
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

        // std::shared_ptr<OperatorChain<std::string>> op_chain = std::make_shared<OperatorChain<std::string>>(stream_task, channel_selector_result_writer, gen_operator_factory);
        std::shared_ptr<OperatorChain<std::string>> op_chain = std::make_shared<OperatorChain<std::string>>(channel_selector_result_writer, gen_operator_factory);

        std::shared_ptr<StreamOperator<std::string>> stream_op = op_chain->get_head_operator();

        (std::dynamic_pointer_cast<OneInputStreamOperator<std::string, std::string>>(stream_op))->process_element(record_1); 
    }


    void testStreamTaskInit( void ) {
        std::cout << "test testStreamTaskInit()" << std::endl;
        std::shared_ptr<AbstractUdfStreamOperator<Function, std::string>> stream_map_1 = std::make_shared<StreamMap<std::string, std::string>>(std::make_shared<StringMapFunction>());
        std::shared_ptr<StreamOperatorFactory<std::string>> operator_factory_1 = SimpleStreamOperatorFactory<std::string>::of(stream_map_1);
        std::shared_ptr<AbstractUdfStreamOperator<Function, std::string>> stream_map_2 = std::make_shared<StreamMap<std::string, std::string>>(std::make_shared<StringMapFunction>());
        std::shared_ptr<StreamOperatorFactory<std::string>> operator_factory_2 = SimpleStreamOperatorFactory<std::string>::of(stream_map_2);

        std::string node_name("string-map");
        // std::shared_ptr<StreamNode<std::string>> src_node = std::make_shared<StreamNode<std::string>>(0, operator_factory_1, node_name);
        // std::shared_ptr<StreamNode<std::string>> target_node = std::make_shared<StreamNode<std::string>>(1, operator_factory_2, node_name);
        std::shared_ptr<StreamNode> src_node = std::make_shared<StreamNode>(0, node_name);
        std::shared_ptr<StreamNode> target_node = std::make_shared<StreamNode>(1, node_name);

        std::shared_ptr<StreamPartitioner<std::string>> stream_partitioner = std::make_shared<ForwardPartitioner<std::string>>();

        std::shared_ptr<StreamEdge<std::string>> edge = std::make_shared<StreamEdge<std::string>>(src_node, target_node, stream_partitioner);


        std::shared_ptr<BufferPool> buffer_pool = std::make_shared<BufferPool>(100, 5);
        std::shared_ptr<ResultPartitionManager> result_partition_manager = std::make_shared<ResultPartitionManager>();

        
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
            input_gates[i] = input_gate_factory.create("test-input-gate", i, std::make_shared<InputGateDeploymentDescriptor>(0, new std::string{"fake-test-task-0"}, 1));
        }
        /* set configuration */
        std::shared_ptr<Configuration> test_conf = std::make_shared<Configuration>();
        test_conf->set_edge<std::string>(StreamConfig::EDGE_NAME, edge);
        test_conf->set_operator_factory<std::string, std::string>(StreamConfig::OPERATOR_FACTORY, operator_factory_1);
        test_conf->set_value<int>(StreamConfig::NUMBER_OF_INPUTS, std::make_shared<int>(1));

        std::shared_ptr<TaskInfo> task_info = std::make_shared<TaskInfo>("test-task", "test-task-0", 0, 1);

        std::shared_ptr<Environment> env = std::make_shared<RuntimeEnvironment>(job_id, job_vertex_id, execution_id, result_partitions, num_of_result_partitions,
                                                                                input_gates, num_of_input_gates, test_conf, task_info);

        /* Create Result Partition */
        // Result partition should initialize first, and the input gate follows it.

        /* Each ResultPartition has 4 Subpartitions */
        std::shared_ptr<ResultPartition> partition_0 = result_partition_factory.create("fake-test-task", 0, 
                                                                                    std::make_shared<ResultPartitionDeploymentDescriptor>(4), buffer_pool);
        
        std::shared_ptr<StreamTask<std::string>> stream_task = std::make_shared<OneInputStreamTask<std::string, std::string>>(env);
        stream_task->before_invoke();

        std::shared_ptr<ResultWriter<std::string>> result_writer_0 = std::make_shared<ResultWriter<std::string>>(partition_0, "test");
        std::shared_ptr<StreamRecord<std::string>> record_1 = std::make_shared<StreamRecord<std::string>>(std::make_shared<std::string>("1234", 4));
        std::shared_ptr<StreamRecord<std::string>> record_2 = std::make_shared<StreamRecord<std::string>>(std::make_shared<std::string>("13456", 5));


        result_writer_0->emit(record_1, 0);
        result_writer_0->emit(record_2, 0);
        // result_writer_0->flush(0);

        stream_task->process_input();
        stream_task->process_input();
    }


    void testSourceStreamTaskInit( void ) {
        std::cout << "test testSourceStreamTaskInit()" << std::endl;
        std::shared_ptr<StreamSource<std::string>> stream_source = std::make_shared<StreamSource<std::string>>(std::make_shared<MySourceFunction>());
        std::shared_ptr<StreamOperatorFactory<std::string>> operator_factory_1 = SimpleStreamOperatorFactory<std::string>::of(stream_source);
        std::shared_ptr<AbstractUdfStreamOperator<Function, std::string>> stream_map_2 = std::make_shared<StreamMap<std::string, std::string>>(std::make_shared<StringMapFunction>());
        std::shared_ptr<StreamOperatorFactory<std::string>> operator_factory_2 = SimpleStreamOperatorFactory<std::string>::of(stream_map_2);

        std::string node_name_1("source");
        std::string node_name_2("map-1");
        std::shared_ptr<StreamNode> src_node = std::make_shared<StreamNode>(0, node_name_1);
        std::shared_ptr<StreamNode> target_node = std::make_shared<StreamNode>(1, node_name_2);
        // std::shared_ptr<StreamNode<std::string>> src_node = std::make_shared<StreamNode<std::string>>(0, operator_factory_1, node_name_1);
        // std::shared_ptr<StreamNode<std::string>> target_node = std::make_shared<StreamNode<std::string>>(1, operator_factory_2, node_name_2);

        std::shared_ptr<StreamPartitioner<std::string>> stream_partitioner = std::make_shared<ForwardPartitioner<std::string>>();

        std::shared_ptr<StreamEdge<std::string>> edge = std::make_shared<StreamEdge<std::string>>(src_node, target_node, stream_partitioner);


        std::shared_ptr<BufferPool> buffer_pool = std::make_shared<BufferPool>(100, 5);
        std::shared_ptr<ResultPartitionManager> result_partition_manager = std::make_shared<ResultPartitionManager>();

        int* partition_idxs = new int[5] {1};
        
        ResultPartitionFactory result_partition_factory(result_partition_manager);

        /* Create RuntimeEnvironment */
        int job_id = 0;
        int job_vertex_id = 0;
        int execution_id = 0;
        int num_of_result_partitions = 1;
        int num_of_input_gates = 0;
        std::shared_ptr<ResultPartition> *result_partitions = new std::shared_ptr<ResultPartition>[num_of_result_partitions];
        std::shared_ptr<InputGate> *input_gates = nullptr; // source task no input gates
        for (int i = 0; i < num_of_result_partitions; i++) {
            result_partitions[i] =  result_partition_factory.create("test-result-partition", i, std::make_shared<ResultPartitionDeploymentDescriptor>(1), buffer_pool);
        }
       
        /* set configuration */
        std::shared_ptr<Configuration> test_conf = std::make_shared<Configuration>();
        test_conf->set_edge<std::string>(StreamConfig::EDGE_NAME, edge);
        test_conf->set_operator_factory<std::string, std::string>(StreamConfig::OPERATOR_FACTORY, operator_factory_1);
        test_conf->set_value<int>(StreamConfig::NUMBER_OF_INPUTS, std::make_shared<int>(1));

        std::shared_ptr<TaskInfo> task_info = std::make_shared<TaskInfo>("test-source-task", "test-source-task-0", 0, 1);

        std::shared_ptr<Environment> env = std::make_shared<RuntimeEnvironment>(job_id, job_vertex_id, execution_id, result_partitions, num_of_result_partitions,
                                                                                input_gates, num_of_input_gates, test_conf, task_info);

        /* Create Result Partition */
        // Result partition should initialize first, and the input gate follows it.

        /* Each ResultPartition has 4 Subpartitions */
        std::shared_ptr<ResultPartition> partition_0 = result_partition_factory.create("test-result-partition-1", 1, 
                                                                                    std::make_shared<ResultPartitionDeploymentDescriptor>(4), buffer_pool);
        
        std::shared_ptr<StreamTask<std::string>> stream_task = std::make_shared<SourceStreamTask<std::string>>(env);
        stream_task->before_invoke();

        stream_task->process_input();
    }

    void testSinkStreamTaskInit( void ){
        std::cout << "test testSinkStreamTaskInit()" << std::endl;
        std::shared_ptr<StreamSink<std::string>> sink_operator = std::make_shared<StreamSink<std::string>>(std::make_shared<MySinkFunction>());
        std::shared_ptr<StreamOperatorFactory<std::string>> operator_factory = SimpleStreamOperatorFactory<std::string>::of(sink_operator);

        // sink operator no out-edges

        std::shared_ptr<BufferPool> buffer_pool = std::make_shared<BufferPool>(100, 5);
        std::shared_ptr<ResultPartitionManager> result_partition_manager = std::make_shared<ResultPartitionManager>();

        int* partition_idxs = new int[5] {1};
        
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
        std::shared_ptr<StreamRecord<std::string>> record_1 = std::make_shared<StreamRecord<std::string>>(std::make_shared<std::string>("1234", 4));
        std::shared_ptr<StreamRecord<std::string>> record_2 = std::make_shared<StreamRecord<std::string>>(std::make_shared<std::string>("13456", 5));


        result_writer_0->emit(record_1, 0);
        result_writer_0->emit(record_2, 0);

        stream_task->process_input();
        stream_task->process_input();
    }

    void testSourceTaskConnectStreamTask( void ) {
        std::cout << "test testSourceTaskConnectStreamTask()" << std::endl;
        std::shared_ptr<StreamSource<std::string>> stream_source = std::make_shared<StreamSource<std::string>>(std::make_shared<MySourceFunction>());
        std::shared_ptr<StreamOperatorFactory<std::string>> operator_factory_1 = SimpleStreamOperatorFactory<std::string>::of(stream_source);
        std::shared_ptr<AbstractUdfStreamOperator<Function, std::string>> stream_map_2 = std::make_shared<StreamMap<std::string, std::string>>(std::make_shared<StringMapFunction>());
        std::shared_ptr<StreamOperatorFactory<std::string>> operator_factory_2 = SimpleStreamOperatorFactory<std::string>::of(stream_map_2);
        std::shared_ptr<AbstractUdfStreamOperator<Function, std::string>> stream_map_3 = std::make_shared<StreamMap<std::string, std::string>>(std::make_shared<StringMapFunction>());
        std::shared_ptr<StreamOperatorFactory<std::string>> operator_factory_3 = SimpleStreamOperatorFactory<std::string>::of(stream_map_3);

        std::string node_name_1("source");
        std::string node_name_2("map-1");
        std::string node_name_3("map-1");

        
        // std::shared_ptr<StreamNode<std::string>> node_1 = std::make_shared<StreamNode<std::string>>(0, operator_factory_1, node_name_1);
        // std::shared_ptr<StreamNode<std::string>> node_2 = std::make_shared<StreamNode<std::string>>(1, operator_factory_2, node_name_2);
        // std::shared_ptr<StreamNode<std::string>> node_3 = std::make_shared<StreamNode<std::string>>(2, operator_factory_3, node_name_3);

        std::shared_ptr<StreamNode> node_1 = std::make_shared<StreamNode>(0, node_name_1);
        std::shared_ptr<StreamNode> node_2 = std::make_shared<StreamNode>(1, node_name_2);
        std::shared_ptr<StreamNode> node_3 = std::make_shared<StreamNode>(2, node_name_3);

        /* Create edge_1 (source -> map-1) */
        std::shared_ptr<StreamEdge<std::string>> edge_1 = std::make_shared<StreamEdge<std::string>>(node_1, node_2, std::make_shared<ForwardPartitioner<std::string>>());
        /* Create edge_2 (map-1 -> map-2) */
        std::shared_ptr<StreamEdge<std::string>> edge_2 = std::make_shared<StreamEdge<std::string>>(node_2, node_3, std::make_shared<ForwardPartitioner<std::string>>());


        std::shared_ptr<BufferPool> buffer_pool = std::make_shared<BufferPool>(200, 3);
        std::shared_ptr<ResultPartitionManager> result_partition_manager = std::make_shared<ResultPartitionManager>();

        /* InputGate and ResultPartition factory */
        InputGateFactory input_gate_factory(result_partition_manager);

        ResultPartitionFactory result_partition_factory(result_partition_manager);

        /* Create runtime environment for StreamTask_1 */
        /* Create RuntimeEnvironment */
        int job_id = 0;
        int job_vertex_id = 0;
        int execution_id = 0;
        int num_of_result_partitions = 1;
        int num_of_input_gates = 0; // Source Task no input gate
        std::shared_ptr<ResultPartition> *result_partitions_1 = new std::shared_ptr<ResultPartition>[num_of_result_partitions];
        for (int i = 0; i < num_of_result_partitions; i++) {
            result_partitions_1[i] =  result_partition_factory.create("test-source-task", i, std::make_shared<ResultPartitionDeploymentDescriptor>(1), buffer_pool);
        }
        
        /* set configuration */
        std::shared_ptr<Configuration> conf_1 = std::make_shared<Configuration>();
        conf_1->set_edge<std::string>(StreamConfig::EDGE_NAME, edge_1);
        conf_1->set_operator_factory<std::string, std::string>(StreamConfig::OPERATOR_FACTORY, operator_factory_1);
        conf_1->set_value<int>(StreamConfig::NUMBER_OF_INPUTS, std::make_shared<int>(1));

        std::shared_ptr<TaskInfo> task_info_1 = std::make_shared<TaskInfo>("test-source-task", "test-source-task-0", 0, 1);

        std::shared_ptr<Environment> env_1 = std::make_shared<RuntimeEnvironment>(job_id, job_vertex_id, execution_id, result_partitions_1, num_of_result_partitions,
                                                                                nullptr, num_of_input_gates, conf_1, task_info_1);

        /* Create runtime environment for StreamTask_2 */
        job_id = 1;
        job_vertex_id = 1;
        execution_id = 1;
        num_of_result_partitions = 1;
        num_of_input_gates = 1;
        std::shared_ptr<ResultPartition> *result_partitions_2 = new std::shared_ptr<ResultPartition>[num_of_result_partitions];
        std::shared_ptr<InputGate> *input_gates_2 = new std::shared_ptr<InputGate>[num_of_input_gates];

        for (int i = 0; i < num_of_result_partitions; i++) {
            // source-task use partition_idx: 0
            result_partitions_2[i] =  result_partition_factory.create("test-map-task", i, std::make_shared<ResultPartitionDeploymentDescriptor>(1), buffer_pool);
        }
        for (int i = 0; i < num_of_input_gates; i++) {
            // connect to source-task's partition_idx: 0
            input_gates_2[i] = input_gate_factory.create("test-map-task", i, std::make_shared<InputGateDeploymentDescriptor>(0, new std::string{"test-source-task-0"}, 1));
        }

        /* set configuration */
        std::shared_ptr<Configuration> conf_2 = std::make_shared<Configuration>();
        conf_2->set_edge<std::string>(StreamConfig::EDGE_NAME, edge_2);
        conf_2->set_operator_factory<std::string, std::string>(StreamConfig::OPERATOR_FACTORY, operator_factory_2);
        conf_2->set_value<int>(StreamConfig::NUMBER_OF_INPUTS, std::make_shared<int>(1));

        std::shared_ptr<TaskInfo> task_info_2 = std::make_shared<TaskInfo>("test-map-task", "test-map-task-0", 0, 1);

        std::shared_ptr<Environment> env_2 = std::make_shared<RuntimeEnvironment>(job_id, job_vertex_id, execution_id, result_partitions_2, num_of_result_partitions,
                                                                                input_gates_2, num_of_input_gates, conf_2, task_info_2);

        std::shared_ptr<StreamTask<std::string>> source_task = std::make_shared<SourceStreamTask<std::string>>(env_1);
        std::shared_ptr<StreamTask<std::string>> map_task = std::make_shared<OneInputStreamTask<std::string, std::string>>(env_2);

        source_task->before_invoke();
        map_task->before_invoke();

        source_task->process_input();

        for (int i = 0; i < 10; i++) {
            map_task->process_input();
        }
        
    } 

    void testMailbox( void ) {
        std::shared_ptr<TaskMailbox> mailbox = std::make_shared<TaskMailbox>(nullptr);
        
        std::shared_ptr<MailboxProcessor> mailbox_processor = std::make_shared<MailboxProcessor>(
                                                                        std::make_shared<TestMailboxDefaultAction>(), 
                                                                        mailbox);

        int loop_time = 10;
        for (int i = 0; i < loop_time; i++) {
            mailbox_processor->run_mailbox_step();
        }
        mailbox_processor->all_actions_completed();
        bool step_result = mailbox_processor->run_mailbox_step();
        TS_ASSERT_EQUALS(step_result, false);
    }

    // void testTupleOneInputStreamProcessorCreate(void) {
    //     std::cout << "test testTupleOneInputStreamProcessorCreate()" << std::endl;
    //     std::shared_ptr<OneInputStreamTask<Tuple2<std::string, int>, std::string>> op = 
    //                 std::make_shared<OneInputStreamTask<Tuple2<std::string, int>, std::string>>(nullptr);
    // }

    void testStreamTaskWithTupleData( void ){
        std::cout << "test testStreamTaskWithTupleData()" << std::endl;
        std::shared_ptr<StreamOperator<std::string>> stream_map_1 = std::make_shared<StreamMap<Tuple2<std::string, int>, std::string>>(std::make_shared<TupleMapFunction>());
        std::shared_ptr<StreamOperatorFactory<std::string>> operator_factory_1 = SimpleStreamOperatorFactory<std::string>::of(stream_map_1);
        std::shared_ptr<StreamOperator<std::string>> stream_map_2 = std::make_shared<StreamMap<std::string, std::string>>(std::make_shared<StringMapFunction>());
        std::shared_ptr<StreamOperatorFactory<std::string>> operator_factory_2 = SimpleStreamOperatorFactory<std::string>::of(stream_map_2);

        std::string node_name("string-map");
        std::shared_ptr<StreamNode> src_node = std::make_shared<StreamNode>(0, node_name);
        std::shared_ptr<StreamNode> target_node = std::make_shared<StreamNode>(1, node_name);

        std::shared_ptr<StreamPartitioner<std::string>> stream_partitioner = std::make_shared<ForwardPartitioner<std::string>>();

        std::shared_ptr<StreamEdge<std::string>> edge = std::make_shared<StreamEdge<std::string>>(src_node, target_node, stream_partitioner);


        std::shared_ptr<BufferPool> buffer_pool = std::make_shared<BufferPool>(100, 150);
        std::shared_ptr<ResultPartitionManager> result_partition_manager = std::make_shared<ResultPartitionManager>();

        
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
            input_gates[i] = input_gate_factory.create("test-input-gate", i, std::make_shared<InputGateDeploymentDescriptor>(0, new std::string{"fake-test-task-0"}, 1));
        }
        /* set configuration */
        std::shared_ptr<Configuration> test_conf = std::make_shared<Configuration>();
        test_conf->set_edge<std::string>(StreamConfig::EDGE_NAME, edge);
        test_conf->set_operator_factory<Tuple2<std::string, int>, std::string>(StreamConfig::OPERATOR_FACTORY, operator_factory_1);
        test_conf->set_value<int>(StreamConfig::NUMBER_OF_INPUTS, std::make_shared<int>(1));

        std::shared_ptr<TaskInfo> task_info = std::make_shared<TaskInfo>("test-task", "test-task-0", 0, 1);

        std::shared_ptr<Environment> env = std::make_shared<RuntimeEnvironment>(job_id, job_vertex_id, execution_id, result_partitions, num_of_result_partitions,
                                                                                input_gates, num_of_input_gates, test_conf, task_info);

        /* Create Result Partition */
        // Result partition should initialize first, and the input gate follows it.

        /* Each ResultPartition has 4 Subpartitions */
        std::shared_ptr<ResultPartition> partition_0 = result_partition_factory.create("fake-test-task", 0, 
                                                                                    std::make_shared<ResultPartitionDeploymentDescriptor>(4), buffer_pool);
        
        std::shared_ptr<StreamTask<std::string>> stream_task = std::make_shared<OneInputStreamTask<Tuple2<std::string, int>, std::string>>(env);
        stream_task->before_invoke();

        std::shared_ptr<ResultWriter<Tuple2<std::string, int>>> result_writer_0 = std::make_shared<ResultWriter<Tuple2<std::string, int>>>(partition_0, "test");
        std::shared_ptr<StreamRecord<Tuple2<std::string, int>>> record_1 = std::make_shared<StreamRecord<Tuple2<std::string, int>>>(std::make_shared<Tuple2<std::string, int>>(
                                                                                                std::make_shared<std::string>(std::string("hello world")),
                                                                                                std::make_shared<int>(101)));
        std::shared_ptr<StreamRecord<Tuple2<std::string, int>>> record_2 = std::make_shared<StreamRecord<Tuple2<std::string, int>>>(std::make_shared<Tuple2<std::string, int>>(
                                                                                                std::make_shared<std::string>(std::string("nju")),
                                                                                                std::make_shared<int>(102)));


        result_writer_0->emit(record_1, 0);
    
        stream_task->process_input();

        result_writer_0->emit(record_2, 0);
        // result_writer_0->flush(0);

        stream_task->process_input();
    }

    void testSinkStreamTaskWithFileWrite( void ){
        std::cout << "test testSinkStreamTaskWithFileWrite()" << std::endl;
        std::shared_ptr<OutputFormat<std::string>> format = std::make_shared<TextOutputFormat<std::string>>(Constant::CLINK_BASE + "resource/test_write_string.txt");
        std::shared_ptr<StreamSink<std::string>> sink_operator = std::make_shared<StreamSink<std::string>>(std::make_shared<OutputFormatSinkFunction<std::string>>(format));
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
        std::shared_ptr<StreamRecord<std::string>> record_1 = std::make_shared<StreamRecord<std::string>>(std::make_shared<std::string>("1234", 4));
        std::shared_ptr<StreamRecord<std::string>> record_2 = std::make_shared<StreamRecord<std::string>>(std::make_shared<std::string>("13456", 5));


        result_writer_0->emit(record_1, 0);
        result_writer_0->emit(record_2, 0);

        stream_task->process_input();
        stream_task->process_input();

        stream_task->clearup();


    }
};

