#pragma once

#include "cxxtest/TestSuite.h"
#include "StreamTaskFactory.hpp"
#include "MapFunction.hpp"
#include "AbstractUdfStreamOperator.hpp"
#include "InputGateFactory.hpp"
#include "ResultPartitionFactory.hpp"
#include "RuntimeEnvironment.hpp"
#include "TaskInformation.hpp"
#include "JobInformation.hpp"
#include "ShuffleEnvironment.hpp"
#include "Task.hpp"

class StringMapFunction: public MapFunction<std::string, std::string>{
    std::shared_ptr<std::string> map(std::string& value){
        std::cout << "StringMapFunction::map(): " << value << std::endl;
        return std::make_shared<std::string>(value.begin(), value.begin() + 2);
    }

    char* serialize() override {return (char*)this;}

    std::shared_ptr<MapFunction<std::string, std::string>> deserialize() override { return std::make_shared<StringMapFunction>();}
};

template<class IN, class OUT>
std::shared_ptr<StreamTask<OUT>> create_one_input_stream_task(std::shared_ptr<Environment> env) {
    return std::make_shared<OneInputStreamTask<IN, OUT>>(env);
}

template<class OUT>
std::shared_ptr<StreamTask<OUT>> create_source_stream_task(std::shared_ptr<Environment> env) {
    return std::make_shared<SourceStreamTask<OUT>>(env);
}

class TestStreamTaskFactory: public CxxTest::TestSuite
{
public:
    void testStreamTaskFactory( void ) {
        
        typedef StreamTaskFactory<AbstractInvokable, std::string> MyStreamTaskFactory;
        // std::shared_ptr<MyStreamTaskFactory> stream_task_factory = std::make_shared<MyStreamTaskFactory>();
        MyStreamTaskFactory* stream_task_factory = MyStreamTaskFactory::instance();

        stream_task_factory->register_stream_task(typeid(OneInputStreamTask<std::string, std::string>).name(), 
                                                    StreamTaskFactoryCreator::create_one_input_stream_task<std::string, std::string>);

        stream_task_factory->register_stream_task(typeid(SourceStreamTask<std::string>).name(), 
                                                    StreamTaskFactoryCreator::create_source_stream_task<std::string>);


        //-------------------------------------------
        // Create Environment
        //-------------------------------------------
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
    
        //----------------------------------------------
        // Create StreamTask
        //----------------------------------------------
        std::shared_ptr<ResultPartition> partition_0 = result_partition_factory.create("fake-test-task", 0, 
                                                                                    std::make_shared<ResultPartitionDeploymentDescriptor>(4), buffer_pool);
        
        // std::shared_ptr<StreamTask<std::string>> stream_task = std::make_shared<OneInputStreamTask<std::string, std::string>>(env);
        std::shared_ptr<AbstractInvokable> stream_task = stream_task_factory->create_task(typeid(OneInputStreamTask<std::string, std::string>).name(), env);
        std::dynamic_pointer_cast<StreamTask<std::string>>(stream_task)->before_invoke();

        std::shared_ptr<ResultWriter<std::string>> result_writer_0 = std::make_shared<ResultWriter<std::string>>(partition_0, "test");
        std::shared_ptr<StreamRecord<std::string>> record_1 = std::make_shared<StreamRecord<std::string>>(std::make_shared<std::string>("1234", 4));
        std::shared_ptr<StreamRecord<std::string>> record_2 = std::make_shared<StreamRecord<std::string>>(std::make_shared<std::string>("13456", 5));


        result_writer_0->emit(record_1, 0);
        result_writer_0->emit(record_2, 0);
        // result_writer_0->flush(0);

        std::dynamic_pointer_cast<StreamTask<std::string>>(stream_task)->process_input();
        std::dynamic_pointer_cast<StreamTask<std::string>>(stream_task)->process_input();
    }

    void TestTaskCreateWithStreamTaskFactory( void ) {
        std::cout << "test TestTaskCreateWithStreamTaskFactory()" << std::endl;
        //-------------------------------------------------
        // Initialize the StreamTaskFactory, register all StreamTask class name
        //-------------------------------------------------
        StreamTaskFactory<AbstractInvokable>::instance()->register_stream_task(typeid(OneInputStreamTask<std::string, std::string>).name(),
                                                                                StreamTaskFactoryCreator::create_one_input_stream_task<std::string, std::string>);


        std::shared_ptr<ResultPartitionManager> result_partition_manager = std::make_shared<ResultPartitionManager>();

        /* InputGate and ResultPartition factory */
        std::shared_ptr<InputGateFactory> input_gate_factory = std::make_shared<InputGateFactory>(result_partition_manager);

        std::shared_ptr<ResultPartitionFactory> result_partition_factory = std::make_shared<ResultPartitionFactory>(result_partition_manager);

        /* Create a fake upstream ResultPartition for sending data to Task */
        std::shared_ptr<BufferPool> buffer_pool_2 = std::make_shared<BufferPool>(100, 50);
        std::shared_ptr<ResultPartition> fake_partition = result_partition_factory->create("fake-result-partition-task", 0, 
                                                                                    std::make_shared<ResultPartitionDeploymentDescriptor>(4), buffer_pool_2);

        /* create Task*/
        std::shared_ptr<JobInformation> job_information = std::make_shared<JobInformation>(0, "test-job");

        std::shared_ptr<Configuration> task_configuration = std::make_shared<Configuration>();

        /* init operator factory */
        std::shared_ptr<AbstractUdfStreamOperator<Function, std::string>> stream_map_1 = std::make_shared<StreamMap<std::string, std::string>>(std::make_shared<StringMapFunction>());
        std::shared_ptr<StreamOperatorFactory<std::string>> operator_factory_1 = SimpleStreamOperatorFactory<std::string>::of(stream_map_1);
        std::shared_ptr<AbstractUdfStreamOperator<Function, std::string>> stream_map_2 = std::make_shared<StreamMap<std::string, std::string>>(std::make_shared<StringMapFunction>());
        std::shared_ptr<StreamOperatorFactory<std::string>> operator_factory_2 = SimpleStreamOperatorFactory<std::string>::of(stream_map_2);
        task_configuration->set_operator_factory<std::string, std::string>(StreamConfig::OPERATOR_FACTORY, operator_factory_1);

        /* init edge */
        std::string node_name("string-map");
        std::shared_ptr<StreamNode> src_node = std::make_shared<StreamNode>(0, node_name);
        std::shared_ptr<StreamNode> target_node = std::make_shared<StreamNode>(1, node_name);
        // std::shared_ptr<StreamNode<std::string>> src_node = std::make_shared<StreamNode<std::string>>(0, operator_factory_1, node_name);
        // std::shared_ptr<StreamNode<std::string>> target_node = std::make_shared<StreamNode<std::string>>(1, operator_factory_2, node_name);
        std::shared_ptr<StreamPartitioner<std::string>> stream_partitioner = std::make_shared<ForwardPartitioner<std::string>>();
        std::shared_ptr<StreamEdge<std::string>> edge = std::make_shared<StreamEdge<std::string>>(src_node, target_node, stream_partitioner);

        task_configuration->set_edge<std::string>(StreamConfig::EDGE_NAME, edge);

        task_configuration->set_value<int>(StreamConfig::NUMBER_OF_INPUTS, std::make_shared<int>(1));

        /* create task_information*/
        std::shared_ptr<TaskInformation> task_information = std::make_shared<TaskInformation>(0, "test-map-task", 1, task_configuration,
                                                                                            typeid(OneInputStreamTask<std::string, std::string>).name());


        typedef std::vector<std::shared_ptr<ResultPartitionDeploymentDescriptor>> ResultPartitionDeploymentDescriptorList;
        typedef std::vector<std::shared_ptr<InputGateDeploymentDescriptor>> InputGateDeploymentDescriptorList;

        /* create ResultPartition & InputGate descriptors */
        ResultPartitionDeploymentDescriptorList result_partition_descriptors;
        result_partition_descriptors.push_back(std::make_shared<ResultPartitionDeploymentDescriptor>(4));

        InputGateDeploymentDescriptorList input_gate_descriptors;
        input_gate_descriptors.push_back(std::make_shared<InputGateDeploymentDescriptor>(0, new std::string{"fake-result-partition-task-0"}, 1));
        

        /* Create ShuffleEnvironment */
        std::shared_ptr<ShuffleEnvironment> shuffle_environment = std::make_shared<LocalShuffleEnvironment>(result_partition_manager,
                                                                                                            result_partition_factory,
                                                                                                            input_gate_factory);

        /* Create BufferPool */
        std::shared_ptr<BufferPool> buffer_pool = std::make_shared<BufferPool>(200, 50);
        std::shared_ptr<Task> task = std::make_shared<Task>(job_information, task_information,
                                                            101, 999, 0,
                                                            result_partition_descriptors,
                                                            input_gate_descriptors,
                                                            shuffle_environment,
                                                            buffer_pool,
                                                            nullptr);
    }
};