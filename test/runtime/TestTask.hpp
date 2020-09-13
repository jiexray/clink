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

    std::shared_ptr<SinkFunction<std::string>> deserialize() override { return std::make_shared<MySinkFunction>();}
};



class TestTask: public CxxTest::TestSuite{
public:
    void testTaskInit( void ) {
        std::cout << "test testTaskInit()" << std::endl;
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
        std::shared_ptr<StreamNode<std::string>> src_node = std::make_shared<StreamNode<std::string>>(0, operator_factory_1, node_name);
        std::shared_ptr<StreamNode<std::string>> target_node = std::make_shared<StreamNode<std::string>>(1, operator_factory_2, node_name);
        std::shared_ptr<StreamPartitioner<std::string>> stream_partitioner = std::make_shared<ForwardPartitioner<std::string>>();
        std::shared_ptr<StreamEdge<std::string>> edge = std::make_shared<StreamEdge<std::string>>(src_node, target_node, stream_partitioner);

        task_configuration->set_edge<std::string>(StreamConfig::EDGE_NAME, edge);

        task_configuration->set_value<int>(StreamConfig::NUMBER_OF_INPUTS, std::make_shared<int>(1));

        /* create task_information*/
        std::shared_ptr<TaskInformation> task_information = std::make_shared<TaskInformation>(0, "test-map-task", 1, task_configuration,
                                                                                            "OneInputStreamTask<std::string, std::string>");


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
                                                            buffer_pool);


        // task->start_task_thread();

        // std::this_thread::sleep_for(std::chrono::seconds(1));

        // task->cancel_task();


    }

    void testTastExecutorInit() {
        std::cout << "test testTastExecutorInit()" << std::endl;
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
        std::shared_ptr<StreamNode<std::string>> src_node = std::make_shared<StreamNode<std::string>>(0, operator_factory_1, node_name);
        std::shared_ptr<StreamNode<std::string>> target_node = std::make_shared<StreamNode<std::string>>(1, operator_factory_2, node_name);
        std::shared_ptr<StreamPartitioner<std::string>> stream_partitioner = std::make_shared<ForwardPartitioner<std::string>>();
        std::shared_ptr<StreamEdge<std::string>> edge = std::make_shared<StreamEdge<std::string>>(src_node, target_node, stream_partitioner);

        task_configuration->set_edge<std::string>(StreamConfig::EDGE_NAME, edge);

        task_configuration->set_value<int>(StreamConfig::NUMBER_OF_INPUTS, std::make_shared<int>(1));

        std::shared_ptr<TaskInformation> task_information = std::make_shared<TaskInformation>(0, "test-map-task", 1, task_configuration,
                                                                                            "OneInputStreamTask<std::string, std::string>");


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

    void testTaskChain( void ) {
        std::cout << "test testTaskChain()" << std::endl;
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
        int slot_id_1 = 5;
        int slot_id_2 = 4;
        int allocation_id_1 = 999;
        int allocation_id_2 = 998;
        int job_id = 0;
        task_exeuctor->allocate_slot(slot_id_1, job_id, allocation_id_1);
        task_exeuctor->allocate_slot(slot_id_2, job_id, allocation_id_2);

        /* create JobInformation and TaskInformation */
        std::shared_ptr<JobInformation> job_information = std::make_shared<JobInformation>(job_id, "test-job");

        std::shared_ptr<Configuration> task_configuration_1 = std::make_shared<Configuration>();
        std::shared_ptr<Configuration> task_configuration_2 = std::make_shared<Configuration>();
        std::shared_ptr<Configuration> task_configuration_3 = std::make_shared<Configuration>();

        /* init operator factory */
        std::shared_ptr<StreamSource<std::string>> stream_source = std::make_shared<StreamSource<std::string>>(std::make_shared<MySourceFunction>());
        std::shared_ptr<StreamOperatorFactory<std::string>> operator_factory_1 = SimpleStreamOperatorFactory<std::string>::of(stream_source);
        std::shared_ptr<AbstractUdfStreamOperator<Function, std::string>> stream_map_2 = std::make_shared<StreamMap<std::string, std::string>>(std::make_shared<StringMapFunction>());
        std::shared_ptr<StreamOperatorFactory<std::string>> operator_factory_2 = SimpleStreamOperatorFactory<std::string>::of(stream_map_2);
        std::shared_ptr<AbstractUdfStreamOperator<Function, std::string>> stream_sink_3 = std::make_shared<StreamSink<std::string, std::string>>(std::make_shared<MySinkFunction>());
        std::shared_ptr<StreamOperatorFactory<std::string>> operator_factory_3 = SimpleStreamOperatorFactory<std::string>::of(stream_sink_3);

        task_configuration_1->set_operator_factory<std::string, std::string>(StreamConfig::OPERATOR_FACTORY, operator_factory_1);
        task_configuration_2->set_operator_factory<std::string, std::string>(StreamConfig::OPERATOR_FACTORY, operator_factory_2);
        task_configuration_3->set_operator_factory<std::string, std::string>(StreamConfig::OPERATOR_FACTORY, operator_factory_3);

        // init edge
        std::string node_name_1("source");
        std::string node_name_2("map-1");
        std::string node_name_3("sink");
        std::shared_ptr<StreamNode<std::string>> node_1 = std::make_shared<StreamNode<std::string>>(0, operator_factory_1, node_name_1);
        std::shared_ptr<StreamNode<std::string>> node_2 = std::make_shared<StreamNode<std::string>>(1, operator_factory_2, node_name_2);
        std::shared_ptr<StreamNode<std::string>> node_3 = std::make_shared<StreamNode<std::string>>(2, operator_factory_3, node_name_3);


        /* Create edge_1 (source -> map-1) */
        std::shared_ptr<StreamEdge<std::string>> edge_1 = std::make_shared<StreamEdge<std::string>>(node_1, node_2, std::make_shared<ForwardPartitioner<std::string>>());
        /* Create edge_2 (map-1 -> sink) */
        std::shared_ptr<StreamEdge<std::string>> edge_2 = std::make_shared<StreamEdge<std::string>>(node_2, node_3, std::make_shared<ForwardPartitioner<std::string>>());

        task_configuration_1->set_edge<std::string>(StreamConfig::EDGE_NAME, edge_1);
        task_configuration_2->set_edge<std::string>(StreamConfig::EDGE_NAME, edge_2);
        // sink task no out-edge

        // source task no input
        task_configuration_2->set_value<int>(StreamConfig::NUMBER_OF_INPUTS, std::make_shared<int>(1));
        task_configuration_3->set_value<int>(StreamConfig::NUMBER_OF_INPUTS, std::make_shared<int>(1));

        std::shared_ptr<TaskInformation> task_information_1 = std::make_shared<TaskInformation>(0,                              // job_vertex_id
                                                                                                "test-source-task",             // task_name
                                                                                                1,                              // subtask_number
                                                                                                task_configuration_1,             
                                                                                                "SourceStreamTask<std::string>"); // invokable name

        std::shared_ptr<TaskInformation> task_information_2 = std::make_shared<TaskInformation>(1,                              // job_vertex_id
                                                                                                "test-map-task",                // task_name
                                                                                                1,                              // subtask_number
                                                                                                task_configuration_2,             
                                                                                                "OneInputStreamTask<std::string, std::string>"); // invokable name
        
        std::shared_ptr<TaskInformation> task_information_3 = std::make_shared<TaskInformation>(2,                              // job_vertex_id
                                                                                                "test-sink-task",               // task_name
                                                                                                1,                              // subtask_number
                                                                                                task_configuration_3,             
                                                                                                "OneInputStreamTask<std::string, std::string>"); // invokable name
        


        /* create ResultPartition & InputGate descriptors */
        std::shared_ptr<ResultPartitionDeploymentDescriptor>* result_partitions_1 = new std::shared_ptr<ResultPartitionDeploymentDescriptor>[1];
        result_partitions_1[0] = std::make_shared<ResultPartitionDeploymentDescriptor>(1);
        std::shared_ptr<ResultPartitionDeploymentDescriptor>* result_partitions_2 = new std::shared_ptr<ResultPartitionDeploymentDescriptor>[1];
        result_partitions_2[0] = std::make_shared<ResultPartitionDeploymentDescriptor>(1);


        std::shared_ptr<InputGateDeploymentDescriptor>* input_gates_2 = new std::shared_ptr<InputGateDeploymentDescriptor>[1];
        input_gates_2[0] = std::make_shared<InputGateDeploymentDescriptor>(0, new std::string{"test-source-task (1/1)-0"}, 1);
        std::shared_ptr<InputGateDeploymentDescriptor>* input_gates_3 = new std::shared_ptr<InputGateDeploymentDescriptor>[1];
        input_gates_3[0] = std::make_shared<InputGateDeploymentDescriptor>(0, new std::string{"test-map-task (1/1)-0"}, 1);

        /* Create TaskDeploymentDescriptor */
        std::shared_ptr<TaskDeploymentDescriptor> tdd_1 = std::make_shared<TaskDeploymentDescriptor>(job_information->get_job_id(),
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

        std::shared_ptr<TaskDeploymentDescriptor> tdd_2 = std::make_shared<TaskDeploymentDescriptor>(job_information->get_job_id(),
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

        std::shared_ptr<TaskDeploymentDescriptor> tdd_3 = std::make_shared<TaskDeploymentDescriptor>(job_information->get_job_id(),
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

        /* subtask task*/
        /**
         * Typically, create Task, ResultPartition & InputGate in Task 
         */
        task_exeuctor->submit_task(tdd_1);
        task_exeuctor->submit_task(tdd_2);
        task_exeuctor->submit_task(tdd_3);

        /* start task */
        /**
         * Create StreamTask, start partition_request
         */
        task_exeuctor->start_task(102);
        std::this_thread::sleep_for(std::chrono::seconds(1));
        task_exeuctor->start_task(103);
        std::this_thread::sleep_for(std::chrono::seconds(1));
        task_exeuctor->start_task(104);

        std::this_thread::sleep_for(std::chrono::seconds(5));

        task_exeuctor->cancel_task(102);
        task_exeuctor->cancel_task(103);
        task_exeuctor->cancel_task(104);
    }
};