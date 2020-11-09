#pragma once

#include "cxxtest/TestSuite.h"
#include <memory>
#include <thread>
#include <chrono>
#include <sstream>
#include "Task.hpp"
#include "TaskExecutor.hpp"
#include "LoggerFactory.hpp"
// #include "Tuple2.hpp"
#include "Tuple2V2.hpp"
#include "FlatMapFunction.hpp"
#include "MapFunction.hpp"
#include "FileReadFunctionV2.hpp"
#include "OutputFormatSinkFunction.hpp"
#include "TaskExecutorRunner.hpp"
#include "StringBuf.hpp"

// concat the first char and the last char
class CountFunction: public MapFunction<Tuple2V2<const char*, int, 128, sizeof(int)>, Tuple2V2<const char*, int, 128, sizeof(int)>>{
private:
    std::map<std::string, int> m_counter;
public:
    Tuple2V2<const char*, int, 128, sizeof(int)>* map(Tuple2V2<const char*, int, 128, sizeof(int)>* value) override {
        std::string word(value->f0);
        if(m_counter.find(word) != m_counter.end()) {
            m_counter[word] += (value->f1);
        } else {
            m_counter[word] = value->f1;
        }
        // std::cout << "word {" << *value.f0 << ", " << m_counter[*value.f0] << "}" << std::endl;
        // return std::make_shared<MyTuple>(std::make_shared<std::string>(*value.f0), std::make_shared<int>(m_counter[*value.f0]));
        // std::cout << "word {" << *value.f0 << ", " << m_counter[*value.f0] << "}" << std::endl;
        // return std::make_shared<std::string>("f0: " + (*value.f0) + ", f1: " + std::to_string(*value.f1));

        Tuple2V2<const char*, int, 128, sizeof(int)>* new_record = new Tuple2V2<const char*, int, 128, sizeof(int)>(value->f0, &value->f1);
        return new_record;
    }

    char* serialize() override {return (char*)this;}

    std::shared_ptr<MapFunction<Tuple2V2<const char*, int, 128, sizeof(int)>, Tuple2V2<const char*, int, 128, sizeof(int)>>> 
          deserialize() override { return std::make_shared<CountFunction>();}
};

// map one line to multiple single word with (word, 1)
class TokenizeFunction: public FlatMapFunction<StringBuf<32>, Tuple2V2<const char*, int, 128, sizeof(int)>>{
private:
    int m_lines = 0;
public:
    void flat_map(StringBuf<32>* value, std::shared_ptr<Collector<Tuple2V2<const char*, int, 128, sizeof(int)>>> collector) override {
        std::string line(value->c_str);
        std::istringstream iss(line);
        std::string tmp_string;
        m_lines++;
        if (m_lines % 1000 == 0) {
            std::cout << "have processed " << m_lines << " lines" << std::endl;
        }
        while(iss >> tmp_string) {
            int cnt = 1;
            Tuple2V2<const char*, int, 128, sizeof(int)>* new_record = new Tuple2V2<const char*, int, 128, sizeof(int)>(tmp_string.c_str(), &cnt);
            collector->collect(new_record);
        }
    }
    char* serialize() override {return (char*)this;}

    std::shared_ptr<FlatMapFunction<StringBuf<32>, Tuple2V2<const char*, int, 128, sizeof(int)>>> 
          deserialize() override { return std::make_shared<TokenizeFunction>();}
};

class MySourceFunction: public SourceFunction<StringBuf<32>> {
    void run(std::shared_ptr<SourceContext<StringBuf<32>>> ctx) override{
        // ctx->collect(std::make_shared<std::string>(std::string("resource/wordcount.txt")));
        // const char file_name[32] = "resource/wordcount.txt";
        StringBuf<32> file_name("resource/Data-1G.txt");
        // const char* file_name = "resource/Data-2G.txt";
        ctx->collect(&file_name);
    }

    char* serialize() override {return (char*)this;}

    std::shared_ptr<SourceFunction<StringBuf<32>>> deserialize() override { return std::make_shared<MySourceFunction>();}
};

class MySinkFunction: public SinkFunction<Tuple2V2<const char*, int, 128, sizeof(int)>> {
    void invoke(Tuple2V2<const char*, int, 128, sizeof(int)>* value) override{
        // std::cout << "tuple: " << value->to_string() << std::endl;
        return;
    }

    char* serialize() override {return (char*)this;}

    std::shared_ptr<SinkFunction<Tuple2V2<const char*, int, 128, sizeof(int)>>> deserialize(char* des_ptr) override { return std::make_shared<MySinkFunction>();}
};

class MySinkFunctionString: public SinkFunction<StringBuf<32>> {
    int m_lines = 0;

    void invoke(StringBuf<32>* value) override {
        m_lines++;
        if (m_lines % 1000 == 0) {
            std::cout << "have processed " << m_lines << " lines" << std::endl;
        }
        return;
    }

    char* serialize() override {return (char*)this;}

    std::shared_ptr<SinkFunction<StringBuf<32>>> deserialize(char* des_ptr) override { return std::make_shared<MySinkFunctionString>();}
};



class TestFileReadThroughput: public CxxTest::TestSuite{
    typedef std::shared_ptr<Configuration> ConfigurationPtr;
public:
    void testWordCount( void ) {
        std::cout << "test testWordCount()" << std::endl;
        //---------------------------------------------------
        // Initialize StreamTaskFactory
        //---------------------------------------------------
        typedef Tuple2V2<const char*, int, 128, sizeof(int)> MyTuple;
        StreamTaskFactory<>::instance()->register_stream_task(typeid(SourceStreamTask<StringBuf<32>>).name(), StreamTaskFactoryCreator::create_source_stream_task<StringBuf<32>>);
        // StreamTaskFactory<>::instance()->register_stream_task(typeid(OneInputStreamTask<const char*, const char*>).name(), StreamTaskFactoryCreator::create_one_input_stream_task<const char*, const char*>);
        StreamTaskFactory<>::instance()->register_stream_task(typeid(OneInputStreamTask<StringBuf<32>, MyTuple>).name(), StreamTaskFactoryCreator::create_one_input_stream_task<StringBuf<32>, MyTuple>);
        // StreamTaskFactory<>::instance()->register_stream_task(typeid(OneInputStreamTask<const char*, MyTuple>).name(), StreamTaskFactoryCreator::create_one_input_stream_task<const char*, MyTuple>);
        StreamTaskFactory<>::instance()->register_stream_task(typeid(OneInputStreamTask<MyTuple, MyTuple>).name(), StreamTaskFactoryCreator::create_one_input_stream_task<MyTuple, MyTuple>);
        StreamTaskFactory<>::instance()->register_stream_task(typeid(OneInputStreamTask<MyTuple>).name(), StreamTaskFactoryCreator::create_one_input_stream_task<MyTuple>);
        StreamTaskFactory<>::instance()->register_stream_task(typeid(OneInputStreamTask<StringBuf<32>>).name(), StreamTaskFactoryCreator::create_one_input_stream_task<StringBuf<32>>);

        // create TaskExecutor
        ConfigurationPtr task_executor_configuration = nullptr;
        std::shared_ptr<TaskExecutorRunner> task_executor_runner = std::make_shared<TaskExecutorRunner>(nullptr, "tm-1");
        std::shared_ptr<TaskExecutor> task_exeuctor = task_executor_runner->get_task_executor();


        /* allocate slot */
        int slot_id_1 = 5;
        int slot_id_2 = 4;
        int allocation_id_1 = 999;
        int allocation_id_2 = 998;
        int job_id = 0;
        task_exeuctor->allocate_slot(slot_id_1, job_id, allocation_id_1);
        task_exeuctor->allocate_slot(slot_id_2, job_id, allocation_id_2);

        /* create JobInformation and TaskInformation */
        std::shared_ptr<JobInformation> job_information = std::make_shared<JobInformation>(job_id, "wordcount");

        std::shared_ptr<Configuration> task_configuration_1 = std::make_shared<Configuration>();
        std::shared_ptr<Configuration> task_configuration_2 = std::make_shared<Configuration>();
        std::shared_ptr<Configuration> task_configuration_3 = std::make_shared<Configuration>();
        std::shared_ptr<Configuration> task_configuration_4 = std::make_shared<Configuration>();
        std::shared_ptr<Configuration> task_configuration_5 = std::make_shared<Configuration>();

        /* init operator factory */
        std::shared_ptr<StreamSource<StringBuf<32>>> stream_source = std::make_shared<StreamSource<StringBuf<32>>>(std::make_shared<MySourceFunction>());
        std::shared_ptr<StreamOperatorFactory<StringBuf<32>>> operator_factory_1 = SimpleStreamOperatorFactory<StringBuf<32>>::of(stream_source);
        std::shared_ptr<StreamOperator<MyTuple>> stream_read_file = std::make_shared<StreamFlatMap<StringBuf<32>, MyTuple>>(std::make_shared<FileReadFunctionV2>());
        std::shared_ptr<StreamOperatorFactory<MyTuple>> operator_factory_2 = SimpleStreamOperatorFactory<MyTuple>::of(stream_read_file); 
        std::shared_ptr<StreamOperator<MyTuple>> stream_string_tokenize = std::make_shared<StreamFlatMap<StringBuf<32>, MyTuple>>(std::make_shared<TokenizeFunction>());
        std::shared_ptr<StreamOperatorFactory<MyTuple>> operator_factory_3 = SimpleStreamOperatorFactory<MyTuple>::of(stream_string_tokenize); 
        std::shared_ptr<StreamOperator<MyTuple>> stream_counter = 
                                            std::make_shared<StreamMap<MyTuple, MyTuple>>(std::make_shared<CountFunction>());
        std::shared_ptr<StreamOperatorFactory<MyTuple>> operator_factory_4 = SimpleStreamOperatorFactory<MyTuple>::of(stream_counter);

        std::shared_ptr<TextOutputFormat<MyTuple>> format = 
                        std::make_shared<TextOutputFormat<MyTuple>>(Constant::CLINK_BASE + "/resource/wordcount_result.txt"); 
        std::shared_ptr<StreamOperator<>> stream_sink = 
                                            std::make_shared<StreamSink<MyTuple>>(std::make_shared<MySinkFunction>());
        std::shared_ptr<StreamOperatorFactory<>> operator_factory_5 = SimpleStreamOperatorFactory<>::of(stream_sink);

        task_configuration_1->set_operator_factory<StringBuf<32>, StringBuf<32>>(StreamConfig::OPERATOR_FACTORY, operator_factory_1);
        task_configuration_2->set_operator_factory<StringBuf<32>, MyTuple>(StreamConfig::OPERATOR_FACTORY, operator_factory_2);
        task_configuration_3->set_operator_factory<StringBuf<32>, MyTuple>(StreamConfig::OPERATOR_FACTORY, operator_factory_3);
        task_configuration_4->set_operator_factory<MyTuple, MyTuple>(StreamConfig::OPERATOR_FACTORY, operator_factory_4);
        task_configuration_5->set_operator_factory<MyTuple>(StreamConfig::OPERATOR_FACTORY, operator_factory_5);

        task_configuration_1->set_value<std::string>(StreamConfig::OPERATOR_ID, std::make_shared<std::string>("op-1"));
        task_configuration_2->set_value<std::string>(StreamConfig::OPERATOR_ID, std::make_shared<std::string>("op-2"));
        task_configuration_3->set_value<std::string>(StreamConfig::OPERATOR_ID, std::make_shared<std::string>("op-3"));
        task_configuration_4->set_value<std::string>(StreamConfig::OPERATOR_ID, std::make_shared<std::string>("op-4"));
        task_configuration_5->set_value<std::string>(StreamConfig::OPERATOR_ID, std::make_shared<std::string>("op-5"));

        task_configuration_1->set_value<std::string>(StreamConfig::OPERATOR_NAME, std::make_shared<std::string>("source-op"));
        task_configuration_2->set_value<std::string>(StreamConfig::OPERATOR_NAME, std::make_shared<std::string>("file-read-op"));
        task_configuration_3->set_value<std::string>(StreamConfig::OPERATOR_NAME, std::make_shared<std::string>("tokenize-op"));
        task_configuration_4->set_value<std::string>(StreamConfig::OPERATOR_NAME, std::make_shared<std::string>("counter-op"));
        task_configuration_5->set_value<std::string>(StreamConfig::OPERATOR_NAME, std::make_shared<std::string>("counter-op"));


        // init edge
        std::string node_name_1("source");
        std::string node_name_2("file-read");
        std::string node_name_3("string-tokenize");
        std::string node_name_4("counter");
        std::string node_name_5("sink");
        // std::shared_ptr<StreamNode<MyTuple>> node_1 = std::make_shared<StreamNode<MyTuple>>(0, operator_factory_1, node_name_1);
        // std::shared_ptr<StreamNode<std::string>> node_2 = std::make_shared<StreamNode<std::string>>(1, operator_factory_2, node_name_2);
        // std::shared_ptr<StreamNode<std::string>> node_3 = std::make_shared<StreamNode<std::string>>(2, operator_factory_3, node_name_3);

        std::shared_ptr<StreamNode> node_1 = std::make_shared<StreamNode>(0, node_name_1);
        std::shared_ptr<StreamNode> node_2 = std::make_shared<StreamNode>(1, node_name_2);
        std::shared_ptr<StreamNode> node_3 = std::make_shared<StreamNode>(2, node_name_3);
        std::shared_ptr<StreamNode> node_4 = std::make_shared<StreamNode>(3, node_name_4);
        std::shared_ptr<StreamNode> node_5 = std::make_shared<StreamNode>(4, node_name_5);


        /* Create edge_1 (source -> file-read) */
        std::shared_ptr<StreamEdge<StringBuf<32>>> edge_1 = std::make_shared<StreamEdge<StringBuf<32>>>(node_1, node_2, std::make_shared<ForwardPartitioner<StringBuf<32>>>());
        /* Create edge_2 (file-raed -> tokenize) */
        // std::shared_ptr<StreamEdge<std::string>> edge_2 = std::make_shared<StreamEdge<std::string>>(node_2, node_3, std::make_shared<ForwardPartitioner<std::string>>());
        // /* Create edge_3 (tokenize -> count) */
        // std::shared_ptr<StreamEdge<MyTuple>> edge_3 = std::make_shared<StreamEdge<MyTuple>>(node_3, node_4, std::make_shared<ForwardPartitioner<MyTuple>>());
        // /* Create edge_4 (count -> sink) */
        // std::shared_ptr<StreamEdge<MyTuple>> edge_4 = std::make_shared<StreamEdge<MyTuple>>(node_4, node_5, std::make_shared<ForwardPartitioner<MyTuple>>());

        /* Createa edge_2 (file_read -> sink) */
        std::shared_ptr<StreamEdge<MyTuple>> edge_2 = std::make_shared<StreamEdge<MyTuple>>(node_2, node_5, std::make_shared<ForwardPartitioner<MyTuple>>());


        task_configuration_1->set_edge<StringBuf<32>>(StreamConfig::EDGE_NAME, edge_1);
        task_configuration_2->set_edge<MyTuple>(StreamConfig::EDGE_NAME, edge_2);
        // task_configuration_3->set_edge<MyTuple>(StreamConfig::EDGE_NAME, edge_3);
        // task_configuration_4->set_edge<MyTuple>(StreamConfig::EDGE_NAME, edge_4);
        // sink task no out-edge

        // source task no input
        task_configuration_2->set_value<int>(StreamConfig::NUMBER_OF_INPUTS, std::make_shared<int>(1));
        // task_configuration_3->set_value<int>(StreamConfig::NUMBER_OF_INPUTS, std::make_shared<int>(1));
        // task_configuration_4->set_value<int>(StreamConfig::NUMBER_OF_INPUTS, std::make_shared<int>(1));
        task_configuration_5->set_value<int>(StreamConfig::NUMBER_OF_INPUTS, std::make_shared<int>(1));

        std::shared_ptr<TaskInformation> task_information_1 = std::make_shared<TaskInformation>(0,                              // job_vertex_id
                                                                                                "source",             // task_name
                                                                                                1,                              // subtask_number
                                                                                                task_configuration_1,             
                                                                                                typeid(SourceStreamTask<StringBuf<32>>).name()); // invokable name

        std::shared_ptr<TaskInformation> task_information_2 = std::make_shared<TaskInformation>(1,                              // job_vertex_id
                                                                                                "file-read",                // task_name
                                                                                                1,                              // subtask_number
                                                                                                task_configuration_2,             
                                                                                                typeid(OneInputStreamTask<StringBuf<32>, MyTuple>).name()); // invokable name
        
        // std::shared_ptr<TaskInformation> task_information_3 = std::make_shared<TaskInformation>(3,                              // job_vertex_id
        //                                                                                         "tokenize",               // task_name
        //                                                                                         1,                              // subtask_number
        //                                                                                         task_configuration_3,             
        //                                                                                         typeid(OneInputStreamTask<std::string, MyTuple>).name()); // invokable name

        // std::shared_ptr<TaskInformation> task_information_4 = std::make_shared<TaskInformation>(4,                              // job_vertex_id
        //                                                                                         "counter",               // task_name
        //                                                                                         1,                              // subtask_number
        //                                                                                         task_configuration_4,             
        //                                                                                         typeid(OneInputStreamTask<MyTuple, MyTuple>).name()); // invokable name
        
        std::shared_ptr<TaskInformation> task_information_5 = std::make_shared<TaskInformation>(5,                              // job_vertex_id
                                                                                                "counter",               // task_name
                                                                                                1,                              // subtask_number
                                                                                                task_configuration_5,             
                                                                                                typeid(OneInputStreamTask<MyTuple>).name()); // invokable name
        


        /* create ResultPartition & InputGate descriptors */
        std::shared_ptr<ResultPartitionDeploymentDescriptor>* result_partitions_1 = new std::shared_ptr<ResultPartitionDeploymentDescriptor>[1];
        result_partitions_1[0] = std::make_shared<ResultPartitionDeploymentDescriptor>(1);
        std::shared_ptr<ResultPartitionDeploymentDescriptor>* result_partitions_2 = new std::shared_ptr<ResultPartitionDeploymentDescriptor>[1];
        result_partitions_2[0] = std::make_shared<ResultPartitionDeploymentDescriptor>(1);
        // std::shared_ptr<ResultPartitionDeploymentDescriptor>* result_partitions_3 = new std::shared_ptr<ResultPartitionDeploymentDescriptor>[1];
        // result_partitions_3[0] = std::make_shared<ResultPartitionDeploymentDescriptor>(1);
        // std::shared_ptr<ResultPartitionDeploymentDescriptor>* result_partitions_4 = new std::shared_ptr<ResultPartitionDeploymentDescriptor>[1];
        // result_partitions_4[0] = std::make_shared<ResultPartitionDeploymentDescriptor>(1);


        std::shared_ptr<InputGateDeploymentDescriptor>* input_gates_2 = new std::shared_ptr<InputGateDeploymentDescriptor>[1];
        input_gates_2[0] = std::make_shared<InputGateDeploymentDescriptor>(0, new std::string{"source (1/1)-0"}, 1);
        // std::shared_ptr<InputGateDeploymentDescriptor>* input_gates_3 = new std::shared_ptr<InputGateDeploymentDescriptor>[1];
        // input_gates_3[0] = std::make_shared<InputGateDeploymentDescriptor>(0, new std::string{"file-read (1/1)-0"}, 1);
        // std::shared_ptr<InputGateDeploymentDescriptor>* input_gates_4 = new std::shared_ptr<InputGateDeploymentDescriptor>[1];
        // input_gates_4[0] = std::make_shared<InputGateDeploymentDescriptor>(0, new std::string{"tokenize (1/1)-0"}, 1);
        std::shared_ptr<InputGateDeploymentDescriptor>* input_gates_5 = new std::shared_ptr<InputGateDeploymentDescriptor>[1];
        input_gates_5[0] = std::make_shared<InputGateDeploymentDescriptor>(0, new std::string{"file-read (1/1)-0"}, 1);

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

        // std::shared_ptr<TaskDeploymentDescriptor> tdd_3 = std::make_shared<TaskDeploymentDescriptor>(job_information->get_job_id(),
        //                                                                                             job_information,
        //                                                                                             task_information_3,
        //                                                                                             104, // execution id
        //                                                                                             allocation_id_2, 
        //                                                                                             0,   // subtask idx
        //                                                                                             5,   // target slot number
        //                                                                                             1,   // number_of_result_partitions
        //                                                                                             1,   // number_of_input_gates
        //                                                                                             result_partitions_3, // result_partitions
        //                                                                                             input_gates_3 // input_gates
        //                                                                                             );

        // std::shared_ptr<TaskDeploymentDescriptor> tdd_4 = std::make_shared<TaskDeploymentDescriptor>(job_information->get_job_id(),
        //                                                                                             job_information,
        //                                                                                             task_information_4,
        //                                                                                             105, // execution id
        //                                                                                             allocation_id_2, 
        //                                                                                             0,   // subtask idx
        //                                                                                             5,   // target slot number
        //                                                                                             1,   // number_of_result_partitions
        //                                                                                             1,   // number_of_input_gates
        //                                                                                             result_partitions_4, // result_partitions
        //                                                                                             input_gates_4 // input_gates
        //                                                                                             );    

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
        // task_exeuctor->submit_task(tdd_3);
        // task_exeuctor->submit_task(tdd_4);
        task_exeuctor->submit_task(tdd_5);

        /* start task */
        /**
         * Create StreamTask, start partition_request
         */
        task_exeuctor->start_task(102);
        task_exeuctor->start_task(103);
        // task_exeuctor->start_task(104);
        // task_exeuctor->start_task(105);
        task_exeuctor->start_task(106);

        std::this_thread::sleep_for(std::chrono::seconds(3600));

        std::cout << "start to finish tasks" << std::endl;

        // task_exeuctor->cancel_task(102);
        task_exeuctor->cancel_task(103);
        // task_exeuctor->cancel_task(104);
        // task_exeuctor->cancel_task(105);
        task_exeuctor->cancel_task(106);
    }
};