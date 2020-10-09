#pragma once

#include "cxxtest/TestSuite.h"
#include <memory>
#include <thread>
#include <chrono>
#include "Task.hpp"
#include "TaskExecutor.hpp"
#include "LoggerFactory.hpp"
#include "Tuple2.hpp"

// concat the first char and the last char
class TupleMapFunction: public MapFunction<Tuple2<std::string, int>, std::string>{
    std::shared_ptr<std::string> map(Tuple2<std::string, int>& value){
        std::cout << "TupleMapFunction::map() " << std::string("f0: " + (*value.f0) + ", f1: " + std::to_string(*value.f1)) << std::endl;
        return std::make_shared<std::string>("f0: " + (*value.f0) + ", f1: " + std::to_string(*value.f1));
    }

    char* serialize() override {return (char*)this;}

    std::shared_ptr<MapFunction<Tuple2<std::string, int>, std::string>> deserialize() override { return std::make_shared<TupleMapFunction>();}
};

class MySourceFunction: public SourceFunction<Tuple2<std::string, int>> {
    void run(std::shared_ptr<SourceContext<Tuple2<std::string, int>>> ctx) {
        for(int i = 0; i < 50; i++) {
            std::cout << "Emit record " << i << std::endl;
            if (i < 10) {
                ctx->collect(std::make_shared<Tuple2<std::string, int>>(
                                            std::make_shared<std::string>(std::string("0" + std::to_string(i) + "-test-data")), 
                                            std::make_shared<int>(i)));
            } else {
                ctx->collect(std::make_shared<Tuple2<std::string, int>>(
                                            std::make_shared<std::string>(std::string(std::to_string(i) + "-test-data")), 
                                            std::make_shared<int>(i)));
            }
        }
    }

    char* serialize() override {return (char*)this;}

    std::shared_ptr<SourceFunction<Tuple2<std::string, int>>> deserialize() override { return std::make_shared<MySourceFunction>();}
};

class MySinkFunction: public SinkFunction<std::string> {
    void invoke(std::string& val) override {
        std::cout << "MySinkFunction::invoke(), get " << val << std::endl;
    }

    char* serialize() override {return (char*)this;}

    std::shared_ptr<SinkFunction<std::string>> deserialize(char* no_use) override { return std::make_shared<MySinkFunction>();}
};



class TestTaskChainWithTuple: public CxxTest::TestSuite{
public:
    void testTaskChainWithTuple( void ) {
        std::cout << "test testTaskChainWithTuple()" << std::endl;
        //-------------------------------------------------------
        // Initialize StreamTaskFactory
        //-------------------------------------------------------
        typedef Tuple2<std::string, int> MyTuple;
        StreamTaskFactory<>::instance()->register_stream_task(typeid(SourceStreamTask<MyTuple>).name(), StreamTaskFactoryCreator::create_source_stream_task<MyTuple>);
        StreamTaskFactory<>::instance()->register_stream_task(typeid(OneInputStreamTask<MyTuple, std::string>).name(), StreamTaskFactoryCreator::create_one_input_stream_task<MyTuple, std::string>);
        StreamTaskFactory<>::instance()->register_stream_task(typeid(OneInputStreamTask<std::string>).name(), StreamTaskFactoryCreator::create_one_input_stream_task<std::string>);


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
        std::shared_ptr<StreamSource<Tuple2<std::string, int>>> stream_source = std::make_shared<StreamSource<Tuple2<std::string, int>>>(std::make_shared<MySourceFunction>());
        std::shared_ptr<StreamOperatorFactory<Tuple2<std::string, int>>> operator_factory_1 = SimpleStreamOperatorFactory<Tuple2<std::string, int>>::of(stream_source);
        std::shared_ptr<StreamOperator<std::string>> stream_map_2 = std::make_shared<StreamMap<Tuple2<std::string, int>, std::string>>(std::make_shared<TupleMapFunction>());
        std::shared_ptr<StreamOperatorFactory<std::string>> operator_factory_2 = SimpleStreamOperatorFactory<std::string>::of(stream_map_2);
        std::shared_ptr<StreamOperator<>> stream_sink_3 = std::make_shared<StreamSink<std::string>>(std::make_shared<MySinkFunction>());
        std::shared_ptr<StreamOperatorFactory<>> operator_factory_3 = SimpleStreamOperatorFactory<>::of(stream_sink_3);

        task_configuration_1->set_operator_factory<std::string, Tuple2<std::string, int>>(StreamConfig::OPERATOR_FACTORY, operator_factory_1);
        task_configuration_2->set_operator_factory<Tuple2<std::string, int>, std::string>(StreamConfig::OPERATOR_FACTORY, operator_factory_2);
        task_configuration_3->set_operator_factory<std::string>(StreamConfig::OPERATOR_FACTORY, operator_factory_3);

        // init edge
        std::string node_name_1("source");
        std::string node_name_2("map-1");
        std::string node_name_3("sink");
        // std::shared_ptr<StreamNode<Tuple2<std::string, int>>> node_1 = std::make_shared<StreamNode<Tuple2<std::string, int>>>(0, operator_factory_1, node_name_1);
        // std::shared_ptr<StreamNode<std::string>> node_2 = std::make_shared<StreamNode<std::string>>(1, operator_factory_2, node_name_2);
        // std::shared_ptr<StreamNode<std::string>> node_3 = std::make_shared<StreamNode<std::string>>(2, operator_factory_3, node_name_3);

        std::shared_ptr<StreamNode> node_1 = std::make_shared<StreamNode>(0, node_name_1);
        std::shared_ptr<StreamNode> node_2 = std::make_shared<StreamNode>(1, node_name_2);
        std::shared_ptr<StreamNode> node_3 = std::make_shared<StreamNode>(2, node_name_3);


        /* Create edge_1 (source -> map-1) */
        std::shared_ptr<StreamEdge<Tuple2<std::string, int>>> edge_1 = std::make_shared<StreamEdge<Tuple2<std::string, int>>>(node_1, node_2, std::make_shared<ForwardPartitioner<Tuple2<std::string, int>>>());
        /* Create edge_2 (map-1 -> sink) */
        std::shared_ptr<StreamEdge<std::string>> edge_2 = std::make_shared<StreamEdge<std::string>>(node_2, node_3, std::make_shared<ForwardPartitioner<std::string>>());

        task_configuration_1->set_edge<Tuple2<std::string, int>>(StreamConfig::EDGE_NAME, edge_1);
        task_configuration_2->set_edge<std::string>(StreamConfig::EDGE_NAME, edge_2);
        // sink task no out-edge

        // source task no input
        task_configuration_2->set_value<int>(StreamConfig::NUMBER_OF_INPUTS, std::make_shared<int>(1));
        task_configuration_3->set_value<int>(StreamConfig::NUMBER_OF_INPUTS, std::make_shared<int>(1));

        std::shared_ptr<TaskInformation> task_information_1 = std::make_shared<TaskInformation>(0,                              // job_vertex_id
                                                                                                "test-source-task",             // task_name
                                                                                                1,                              // subtask_number
                                                                                                task_configuration_1,             
                                                                                                typeid(SourceStreamTask<Tuple2<std::string, int>>).name()); // invokable name

        std::shared_ptr<TaskInformation> task_information_2 = std::make_shared<TaskInformation>(1,                              // job_vertex_id
                                                                                                "test-map-task",                // task_name
                                                                                                1,                              // subtask_number
                                                                                                task_configuration_2,             
                                                                                                typeid(OneInputStreamTask<Tuple2<std::string, int>, std::string>).name()); // invokable name
        
        std::shared_ptr<TaskInformation> task_information_3 = std::make_shared<TaskInformation>(2,                              // job_vertex_id
                                                                                                "test-sink-task",               // task_name
                                                                                                1,                              // subtask_number
                                                                                                task_configuration_3,             
                                                                                                typeid(OneInputStreamTask<std::string>).name()); // invokable name
        


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
        // std::this_thread::sleep_for(std::chrono::seconds(1));
        task_exeuctor->start_task(103);
        // std::this_thread::sleep_for(std::chrono::seconds(1));
        task_exeuctor->start_task(104);

        std::this_thread::sleep_for(std::chrono::seconds(3));

        task_exeuctor->cancel_task(102);
        task_exeuctor->cancel_task(103);
        task_exeuctor->cancel_task(104);
    }
};