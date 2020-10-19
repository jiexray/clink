#pragma once
#include "cxxtest/TestSuite.h"
#include <memory>
#include <thread>
#include <chrono>
#include "Task.hpp"
#include "TaskExecutor.hpp"

template class MapFunction<std::string, std::string>;
// concat the first char and the last char
class StringMapFunction: public MapFunction<std::string, std::string>{
    std::shared_ptr<std::string> map(std::string& value){
        return std::make_shared<std::string>(value.begin(), value.begin() + 2);
    }

    char* serialize() override {return (char*)this;}

    std::shared_ptr<MapFunction<std::string, std::string>> deserialize() override { return std::make_shared<StringMapFunction>();}
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



class TestTask: public CxxTest::TestSuite{
public:
    void testTaskInit( void ) {
        std::cout << "test testTaskInit()" << std::endl;
        // Initialize StreamTaskFactory
        StreamTaskFactory<>::instance()->register_stream_task(typeid(OneInputStreamTask<std::string, std::string>).name(), StreamTaskFactoryCreator::create_one_input_stream_task<std::string, std::string>);

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


        // task->start_task_thread();

        // std::this_thread::sleep_for(std::chrono::seconds(1));

        // task->cancel_task();


    }

    void testTastExecutorInit() {
        std::cout << "test testTastExecutorInit()" << std::endl;
        // Initialize StreamTaskFactory
        StreamTaskFactory<>::instance()->register_stream_task(typeid(OneInputStreamTask<std::string, std::string>).name(), StreamTaskFactoryCreator::create_one_input_stream_task<std::string, std::string>);


        // create ResultParititionManager, ResultPartitionFactory, InputGateFactory
        std::shared_ptr<ResultPartitionManager> result_partition_manager = std::make_shared<ResultPartitionManager>();
        std::shared_ptr<InputGateFactory> input_gate_factory = std::make_shared<InputGateFactory>(result_partition_manager);
        std::shared_ptr<ResultPartitionFactory> result_partition_factory = std::make_shared<ResultPartitionFactory>(result_partition_manager);

        /* create TaskSlotTable */
        std::shared_ptr<TaskSlotTable> task_slot_table = std::make_shared<TaskSlotTable>(10);
        /* create ShuffleEnvironment */
        std::shared_ptr<ShuffleEnvironment> shuffle_environment = std::make_shared<LocalShuffleEnvironment>(result_partition_manager,
                                                                                                            result_partition_factory,
                                                                                                            input_gate_factory);
        std::shared_ptr<TaskExecutor> task_exeuctor = std::make_shared<TaskExecutor>(task_slot_table, shuffle_environment);


        /* allocate slot */
        int slot_id = 5;
        int allocation_id = 999;
        int job_id = 0;
        task_exeuctor->allocate_slot(slot_id, job_id, allocation_id);

        /* create JobInformation and TaskInformation */
        std::shared_ptr<JobInformation> job_information = std::make_shared<JobInformation>(job_id, "test-job");

        std::shared_ptr<Configuration> task_configuration = std::make_shared<Configuration>();

        /* init operator factory */
        std::shared_ptr<AbstractUdfStreamOperator<Function, std::string>> stream_map_1 = std::make_shared<StreamMap<std::string, std::string>>(std::make_shared<StringMapFunction>());
        std::shared_ptr<StreamOperatorFactory<std::string>> operator_factory_1 = SimpleStreamOperatorFactory<std::string>::of(stream_map_1);
        std::shared_ptr<AbstractUdfStreamOperator<Function, std::string>> stream_map_2 = std::make_shared<StreamMap<std::string, std::string>>(std::make_shared<StringMapFunction>());
        std::shared_ptr<StreamOperatorFactory<std::string>> operator_factory_2 = SimpleStreamOperatorFactory<std::string>::of(stream_map_2);
        task_configuration->set_operator_factory<std::string, std::string>(StreamConfig::OPERATOR_FACTORY, operator_factory_1);

        // init edge
        std::string node_name("string-map");
        // std::shared_ptr<StreamNode<std::string>> src_node = std::make_shared<StreamNode<std::string>>(0, operator_factory_1, node_name);
        // std::shared_ptr<StreamNode<std::string>> target_node = std::make_shared<StreamNode<std::string>>(1, operator_factory_2, node_name);
        std::shared_ptr<StreamNode> src_node = std::make_shared<StreamNode>(0, node_name);
        std::shared_ptr<StreamNode> target_node = std::make_shared<StreamNode>(1, node_name);
        std::shared_ptr<StreamPartitioner<std::string>> stream_partitioner = std::make_shared<ForwardPartitioner<std::string>>();
        std::shared_ptr<StreamEdge<std::string>> edge = std::make_shared<StreamEdge<std::string>>(src_node, target_node, stream_partitioner);

        task_configuration->set_edge<std::string>(StreamConfig::EDGE_NAME, edge);

        task_configuration->set_value<int>(StreamConfig::NUMBER_OF_INPUTS, std::make_shared<int>(1));

        std::shared_ptr<TaskInformation> task_information = std::make_shared<TaskInformation>(0, "test-map-task", 1, task_configuration,
                                                                                            typeid(OneInputStreamTask<std::string, std::string>).name());


        /* create ResultPartition & InputGate descriptors */
        std::shared_ptr<ResultPartitionDeploymentDescriptor>* result_partitions = new std::shared_ptr<ResultPartitionDeploymentDescriptor>[1];
        result_partitions[0] = std::make_shared<ResultPartitionDeploymentDescriptor>(4);

        std::shared_ptr<InputGateDeploymentDescriptor>* input_gates = new std::shared_ptr<InputGateDeploymentDescriptor>[1];
        input_gates[0] = std::make_shared<InputGateDeploymentDescriptor>(0, new std::string{"fake-result-partition-task-0"}, 1);

        /* Create TaskDeploymentDescriptor */
        std::shared_ptr<TaskDeploymentDescriptor> tdd = std::make_shared<TaskDeploymentDescriptor>(job_information->get_job_id(),
                                                                                                    job_information,
                                                                                                    task_information,
                                                                                                    102, // execution id
                                                                                                    allocation_id, 
                                                                                                    0,   // subtask idx
                                                                                                    5,   // target slot number
                                                                                                    1,   // number_of_result_partitions
                                                                                                    1,   // number_of_input_gates
                                                                                                    result_partitions,
                                                                                                    input_gates
                                                                                                    );

        TS_ASSERT_EQUALS(102, tdd->get_execution_id());
        TS_ASSERT_EQUALS(0, tdd->get_subtask_idx());
        TS_ASSERT_EQUALS(5, tdd->get_target_slot_number());
        TS_ASSERT_EQUALS(1, tdd->get_number_of_input_gates());
        TS_ASSERT_EQUALS(1, tdd->get_number_of_result_partitions());

        /* Create a fake upstream ResultPartition for sending data to Task */
        std::shared_ptr<BufferPool> buffer_pool = std::make_shared<BufferPool>(100, 50);
        std::shared_ptr<ResultPartition> fake_partition = result_partition_factory->create("fake-result-partition-task", 0, 
                                                                                    std::make_shared<ResultPartitionDeploymentDescriptor>(4), buffer_pool);
        std::shared_ptr<ResultWriter<std::string>> fake_result_writer = std::make_shared<ResultWriter<std::string>>(fake_partition, "fake-result-writer");
        
        /* start task*/
        // task_exeuctor->submit_task(tdd);

        // std::this_thread::sleep_for(std::chrono::seconds(1));

        // task_exeuctor->cancel_task(102);
    }
};