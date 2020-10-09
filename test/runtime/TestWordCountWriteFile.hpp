#pragma once

#include "cxxtest/TestSuite.h"
#include <memory>
#include <thread>
#include <chrono>
#include <sstream>
#include "Task.hpp"
#include "TaskExecutor.hpp"
#include "LoggerFactory.hpp"
#include "Tuple2.hpp"
#include "FlatMapFunction.hpp"
#include "MapFunction.hpp"
#include "FileReadFunction.hpp"
#include "OutputFormatSinkFunction.hpp"


// // concat the first char and the last char
// class CountFunction: public SinkFunction<Tuple2<std::string, int>>{
// private:
//     std::map<std::string, int> m_counter;
// public:
//     void invoke(Tuple2<std::string, int>& value){
//         if(m_counter.find(*value.f0) != m_counter.end()) {
//             m_counter[*value.f0] += (*value.f1);
//         } else {
//             m_counter[*value.f0] = *value.f1;
//         }

//         std::cout << "word {" << *value.f0 << ", " << m_counter[*value.f0] << "}" << std::endl;
//         // return std::make_shared<std::string>("f0: " + (*value.f0) + ", f1: " + std::to_string(*value.f1));
//     }

//     char* serialize() override {return (char*)this;}

//     std::shared_ptr<SinkFunction<Tuple2<std::string, int>>> deserialize(char* no_use) override { return std::make_shared<CountFunction>();}
// };

// concat the first char and the last char
class CountFunction: public MapFunction<Tuple2<std::string, int>, Tuple2<std::string, int>>{
private:
    std::map<std::string, int> m_counter;
public:
    std::shared_ptr<Tuple2<std::string, int>> map(Tuple2<std::string, int>& value){
        if(m_counter.find(*value.f0) != m_counter.end()) {
            m_counter[*value.f0] += (*value.f1);
        } else {
            m_counter[*value.f0] = *value.f1;
        }
        std::cout << "word {" << *value.f0 << ", " << m_counter[*value.f0] << "}" << std::endl;
        return std::make_shared<Tuple2<std::string, int>>(std::make_shared<std::string>(*value.f0), std::make_shared<int>(m_counter[*value.f0]));
        // std::cout << "word {" << *value.f0 << ", " << m_counter[*value.f0] << "}" << std::endl;
        // return std::make_shared<std::string>("f0: " + (*value.f0) + ", f1: " + std::to_string(*value.f1));
    }

    char* serialize() override {return (char*)this;}

    std::shared_ptr<MapFunction<Tuple2<std::string, int>, Tuple2<std::string, int>>> deserialize() override { return std::make_shared<CountFunction>();}
};

// map one line to multiple single word with (word, 1)
class TokenizeFunction: public FlatMapFunction<std::string, Tuple2<std::string, int>>{
public:
    void flat_map(std::shared_ptr<std::string> value, std::shared_ptr<Collector<Tuple2<std::string, int>>> collector) {
        std::istringstream iss(*value);
        std::string tmp_string;
        while(iss >> tmp_string) {
            collector->collect(std::make_shared<Tuple2<std::string, int>>(
                                                                std::make_shared<std::string>(tmp_string),
                                                                std::make_shared<int>(1)));
        }
    }
    char* serialize() override {return (char*)this;}

    std::shared_ptr<FlatMapFunction<std::string, Tuple2<std::string, int>>> deserialize() override { return std::make_shared<TokenizeFunction>();}
};

class MySourceFunction: public SourceFunction<std::string> {
    void run(std::shared_ptr<SourceContext<std::string>> ctx) {
        ctx->collect(std::make_shared<std::string>(std::string("resource/wordcount.txt")));
    }

    char* serialize() override {return (char*)this;}

    std::shared_ptr<SourceFunction<std::string>> deserialize() override { return std::make_shared<MySourceFunction>();}
};



class TestWordCount: public CxxTest::TestSuite{
public:
    void testWordCount( void ) {
        std::cout << "test testWordCount()" << std::endl;
        //---------------------------------------------------
        // Initialize StreamTaskFactory
        //---------------------------------------------------
        typedef Tuple2<std::string, int> MyTuple;
        StreamTaskFactory<>::instance()->register_stream_task(typeid(SourceStreamTask<std::string>).name(), StreamTaskFactoryCreator::create_source_stream_task<std::string>);
        StreamTaskFactory<>::instance()->register_stream_task(typeid(OneInputStreamTask<std::string, std::string>).name(), StreamTaskFactoryCreator::create_one_input_stream_task<std::string, std::string>);
        StreamTaskFactory<>::instance()->register_stream_task(typeid(OneInputStreamTask<std::string, Tuple2<std::string, int>>).name(), StreamTaskFactoryCreator::create_one_input_stream_task<std::string, MyTuple>);
        StreamTaskFactory<>::instance()->register_stream_task(typeid(OneInputStreamTask<Tuple2<std::string, int>, Tuple2<std::string, int>>).name(), StreamTaskFactoryCreator::create_one_input_stream_task<MyTuple, MyTuple>);
        StreamTaskFactory<>::instance()->register_stream_task(typeid(OneInputStreamTask<Tuple2<std::string, int>>).name(), StreamTaskFactoryCreator::create_one_input_stream_task<MyTuple>);

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
        std::shared_ptr<Configuration> task_configuration_4 = std::make_shared<Configuration>();
        std::shared_ptr<Configuration> task_configuration_5 = std::make_shared<Configuration>();

        /* init operator factory */
        std::shared_ptr<StreamSource<std::string>> stream_source = std::make_shared<StreamSource<std::string>>(std::make_shared<MySourceFunction>());
        std::shared_ptr<StreamOperatorFactory<std::string>> operator_factory_1 = SimpleStreamOperatorFactory<std::string>::of(stream_source);
        std::shared_ptr<StreamOperator<std::string>> stream_read_file = std::make_shared<StreamFlatMap<std::string, std::string>>(std::make_shared<FileReadFunction>());
        std::shared_ptr<StreamOperatorFactory<std::string>> operator_factory_2 = SimpleStreamOperatorFactory<std::string>::of(stream_read_file); 
        std::shared_ptr<StreamOperator<Tuple2<std::string, int>>> stream_string_tokenize = std::make_shared<StreamFlatMap<std::string, Tuple2<std::string, int>>>(std::make_shared<TokenizeFunction>());
        std::shared_ptr<StreamOperatorFactory<Tuple2<std::string, int>>> operator_factory_3 = SimpleStreamOperatorFactory<Tuple2<std::string, int>>::of(stream_string_tokenize); 
        std::shared_ptr<StreamOperator<Tuple2<std::string, int>>> stream_counter = 
                                            std::make_shared<StreamMap<Tuple2<std::string, int>, Tuple2<std::string, int>>>(std::make_shared<CountFunction>());
        std::shared_ptr<StreamOperatorFactory<Tuple2<std::string, int>>> operator_factory_4 = SimpleStreamOperatorFactory<Tuple2<std::string, int>>::of(stream_counter);

        std::shared_ptr<TextOutputFormat<Tuple2<std::string, int>>> format = 
                        std::make_shared<TextOutputFormat<Tuple2<std::string, int>>>(Constant::CLINK_BASE + "/resource/wordcount_result.txt"); 
        std::shared_ptr<StreamOperator<>> stream_sink = 
                                            std::make_shared<StreamSink<Tuple2<std::string, int>>>(std::make_shared<OutputFormatSinkFunction<Tuple2<std::string, int>>>(format));
        std::shared_ptr<StreamOperatorFactory<>> operator_factory_5 = SimpleStreamOperatorFactory<>::of(stream_sink);

        task_configuration_1->set_operator_factory<std::string, std::string>(StreamConfig::OPERATOR_FACTORY, operator_factory_1);
        task_configuration_2->set_operator_factory<std::string, std::string>(StreamConfig::OPERATOR_FACTORY, operator_factory_2);
        task_configuration_3->set_operator_factory<std::string, Tuple2<std::string, int>>(StreamConfig::OPERATOR_FACTORY, operator_factory_3);
        task_configuration_4->set_operator_factory<Tuple2<std::string, int>, Tuple2<std::string, int>>(StreamConfig::OPERATOR_FACTORY, operator_factory_4);
        task_configuration_5->set_operator_factory<Tuple2<std::string, int>>(StreamConfig::OPERATOR_FACTORY, operator_factory_5);

        // init edge
        std::string node_name_1("source");
        std::string node_name_2("file-read");
        std::string node_name_3("string-tokenize");
        std::string node_name_4("counter");
        std::string node_name_5("sink");
        // std::shared_ptr<StreamNode<Tuple2<std::string, int>>> node_1 = std::make_shared<StreamNode<Tuple2<std::string, int>>>(0, operator_factory_1, node_name_1);
        // std::shared_ptr<StreamNode<std::string>> node_2 = std::make_shared<StreamNode<std::string>>(1, operator_factory_2, node_name_2);
        // std::shared_ptr<StreamNode<std::string>> node_3 = std::make_shared<StreamNode<std::string>>(2, operator_factory_3, node_name_3);

        std::shared_ptr<StreamNode> node_1 = std::make_shared<StreamNode>(0, node_name_1);
        std::shared_ptr<StreamNode> node_2 = std::make_shared<StreamNode>(1, node_name_2);
        std::shared_ptr<StreamNode> node_3 = std::make_shared<StreamNode>(2, node_name_3);
        std::shared_ptr<StreamNode> node_4 = std::make_shared<StreamNode>(3, node_name_4);
        std::shared_ptr<StreamNode> node_5 = std::make_shared<StreamNode>(4, node_name_5);


        /* Create edge_1 (source -> file-read) */
        std::shared_ptr<StreamEdge<std::string>> edge_1 = std::make_shared<StreamEdge<std::string>>(node_1, node_2, std::make_shared<ForwardPartitioner<std::string>>());
        /* Create edge_2 (file-raed -> tokenize) */
        std::shared_ptr<StreamEdge<std::string>> edge_2 = std::make_shared<StreamEdge<std::string>>(node_2, node_3, std::make_shared<ForwardPartitioner<std::string>>());
        /* Create edge_3 (tokenize -> count) */
        std::shared_ptr<StreamEdge<Tuple2<std::string, int>>> edge_3 = std::make_shared<StreamEdge<Tuple2<std::string, int>>>(node_3, node_4, std::make_shared<ForwardPartitioner<Tuple2<std::string, int>>>());
        /* Create edge_4 (count -> sink) */
        std::shared_ptr<StreamEdge<Tuple2<std::string, int>>> edge_4 = std::make_shared<StreamEdge<Tuple2<std::string, int>>>(node_4, node_5, std::make_shared<ForwardPartitioner<Tuple2<std::string, int>>>());


        task_configuration_1->set_edge<std::string>(StreamConfig::EDGE_NAME, edge_1);
        task_configuration_2->set_edge<std::string>(StreamConfig::EDGE_NAME, edge_2);
        task_configuration_3->set_edge<Tuple2<std::string, int>>(StreamConfig::EDGE_NAME, edge_3);
        task_configuration_4->set_edge<Tuple2<std::string, int>>(StreamConfig::EDGE_NAME, edge_4);
        // sink task no out-edge

        // source task no input
        task_configuration_2->set_value<int>(StreamConfig::NUMBER_OF_INPUTS, std::make_shared<int>(1));
        task_configuration_3->set_value<int>(StreamConfig::NUMBER_OF_INPUTS, std::make_shared<int>(1));
        task_configuration_4->set_value<int>(StreamConfig::NUMBER_OF_INPUTS, std::make_shared<int>(1));
        task_configuration_5->set_value<int>(StreamConfig::NUMBER_OF_INPUTS, std::make_shared<int>(1));

        std::shared_ptr<TaskInformation> task_information_1 = std::make_shared<TaskInformation>(0,                              // job_vertex_id
                                                                                                "test-source-task",             // task_name
                                                                                                1,                              // subtask_number
                                                                                                task_configuration_1,             
                                                                                                typeid(SourceStreamTask<std::string>).name()); // invokable name

        std::shared_ptr<TaskInformation> task_information_2 = std::make_shared<TaskInformation>(1,                              // job_vertex_id
                                                                                                "test-file-read-task",                // task_name
                                                                                                1,                              // subtask_number
                                                                                                task_configuration_2,             
                                                                                                typeid(OneInputStreamTask<std::string, std::string>).name()); // invokable name
        
        std::shared_ptr<TaskInformation> task_information_3 = std::make_shared<TaskInformation>(3,                              // job_vertex_id
                                                                                                "test-tokenize-task",               // task_name
                                                                                                1,                              // subtask_number
                                                                                                task_configuration_3,             
                                                                                                typeid(OneInputStreamTask<std::string, Tuple2<std::string, int>>).name()); // invokable name

        std::shared_ptr<TaskInformation> task_information_4 = std::make_shared<TaskInformation>(4,                              // job_vertex_id
                                                                                                "test-counter-task",               // task_name
                                                                                                1,                              // subtask_number
                                                                                                task_configuration_4,             
                                                                                                typeid(OneInputStreamTask<Tuple2<std::string, int>, Tuple2<std::string, int>>).name()); // invokable name
        
        std::shared_ptr<TaskInformation> task_information_5 = std::make_shared<TaskInformation>(5,                              // job_vertex_id
                                                                                                "test-sink-task",               // task_name
                                                                                                1,                              // subtask_number
                                                                                                task_configuration_5,             
                                                                                                typeid(OneInputStreamTask<Tuple2<std::string, int>>).name()); // invokable name
        


        /* create ResultPartition & InputGate descriptors */
        std::shared_ptr<ResultPartitionDeploymentDescriptor>* result_partitions_1 = new std::shared_ptr<ResultPartitionDeploymentDescriptor>[1];
        result_partitions_1[0] = std::make_shared<ResultPartitionDeploymentDescriptor>(1);
        std::shared_ptr<ResultPartitionDeploymentDescriptor>* result_partitions_2 = new std::shared_ptr<ResultPartitionDeploymentDescriptor>[1];
        result_partitions_2[0] = std::make_shared<ResultPartitionDeploymentDescriptor>(1);
        std::shared_ptr<ResultPartitionDeploymentDescriptor>* result_partitions_3 = new std::shared_ptr<ResultPartitionDeploymentDescriptor>[1];
        result_partitions_3[0] = std::make_shared<ResultPartitionDeploymentDescriptor>(1);
        std::shared_ptr<ResultPartitionDeploymentDescriptor>* result_partitions_4 = new std::shared_ptr<ResultPartitionDeploymentDescriptor>[1];
        result_partitions_4[0] = std::make_shared<ResultPartitionDeploymentDescriptor>(1);


        std::shared_ptr<InputGateDeploymentDescriptor>* input_gates_2 = new std::shared_ptr<InputGateDeploymentDescriptor>[1];
        input_gates_2[0] = std::make_shared<InputGateDeploymentDescriptor>(0, new std::string{"test-source-task (1/1)-0"}, 1);
        std::shared_ptr<InputGateDeploymentDescriptor>* input_gates_3 = new std::shared_ptr<InputGateDeploymentDescriptor>[1];
        input_gates_3[0] = std::make_shared<InputGateDeploymentDescriptor>(0, new std::string{"test-file-read-task (1/1)-0"}, 1);
        std::shared_ptr<InputGateDeploymentDescriptor>* input_gates_4 = new std::shared_ptr<InputGateDeploymentDescriptor>[1];
        input_gates_4[0] = std::make_shared<InputGateDeploymentDescriptor>(0, new std::string{"test-tokenize-task (1/1)-0"}, 1);
        std::shared_ptr<InputGateDeploymentDescriptor>* input_gates_5 = new std::shared_ptr<InputGateDeploymentDescriptor>[1];
        input_gates_5[0] = std::make_shared<InputGateDeploymentDescriptor>(0, new std::string{"test-counter-task (1/1)-0"}, 1);

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
                                                                                                    1,   // number_of_result_partitions
                                                                                                    1,   // number_of_input_gates
                                                                                                    result_partitions_3, // result_partitions
                                                                                                    input_gates_3 // input_gates
                                                                                                    );

        std::shared_ptr<TaskDeploymentDescriptor> tdd_4 = std::make_shared<TaskDeploymentDescriptor>(job_information->get_job_id(),
                                                                                                    job_information,
                                                                                                    task_information_4,
                                                                                                    105, // execution id
                                                                                                    allocation_id_2, 
                                                                                                    0,   // subtask idx
                                                                                                    5,   // target slot number
                                                                                                    1,   // number_of_result_partitions
                                                                                                    1,   // number_of_input_gates
                                                                                                    result_partitions_4, // result_partitions
                                                                                                    input_gates_4 // input_gates
                                                                                                    );    

        std::shared_ptr<TaskDeploymentDescriptor> tdd_5 = std::make_shared<TaskDeploymentDescriptor>(job_information->get_job_id(),
                                                                                                    job_information,
                                                                                                    task_information_5,
                                                                                                    106, // execution id
                                                                                                    allocation_id_2, 
                                                                                                    0,   // subtask idx
                                                                                                    5,   // target slot number
                                                                                                    0,   // number_of_result_partitions
                                                                                                    1,   // number_of_input_gates
                                                                                                    nullptr, // result_partitions
                                                                                                    input_gates_5 // input_gates
                                                                                                    ); 
        /* subtask task*/
        /**
         * Typically, create Task, ResultPartition & InputGate in Task 
         */
        task_exeuctor->submit_task(tdd_1);
        task_exeuctor->submit_task(tdd_2);
        task_exeuctor->submit_task(tdd_3);
        task_exeuctor->submit_task(tdd_4);
        task_exeuctor->submit_task(tdd_5);

        /* start task */
        /**
         * Create StreamTask, start partition_request
         */
        task_exeuctor->start_task(102);
        task_exeuctor->start_task(103);
        task_exeuctor->start_task(104);
        task_exeuctor->start_task(105);
        task_exeuctor->start_task(106);

        std::this_thread::sleep_for(std::chrono::seconds(3));

        task_exeuctor->cancel_task(102);
        task_exeuctor->cancel_task(103);
        task_exeuctor->cancel_task(104);
        task_exeuctor->cancel_task(105);
        task_exeuctor->cancel_task(106);
    }
};