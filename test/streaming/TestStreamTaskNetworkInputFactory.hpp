#pragma once
#include "cxxtest/TestSuite.h"
#include "StreamTaskNetworkInputFactory.hpp"
#include "InputGateFactory.hpp"
#include "ResultPartitionFactory.hpp"
#include "ResultPartitionManager.hpp"
#include "MapFunction.hpp"
#include "ResultWriter.hpp"
#include <string>
#include "StreamMap.hpp"
#include "StreamTaskNetworkInput.hpp"
#include "StreamTaskNetworkOutput.hpp"

class StringMapFunction: public MapFunction<Tuple2<std::string, int>, std::string>{
    std::shared_ptr<std::string> map(Tuple2<std::string, int>& value){
        std::cout << "StringMapFunction::map() " << std::string("f0: " + (*value.f0) + ", f1: " + std::to_string(*value.f1)) << std::endl;
        return std::make_shared<std::string>("f0: " + (*value.f0) + ", f1: " + std::to_string(*value.f1));
    }
};

class TestStreamTaskNetworkInputFactory: public CxxTest::TestSuite {
public:
    void testStreamTaskNetworkInputFactory( void ) {
        std::cout << "test testStreamTaskNetworkInputFactory()" << std::endl;

        std::shared_ptr<InputGateDeploymentDescriptor> input_gate_deployment_descriptor = std::make_shared<InputGateDeploymentDescriptor>(0, new std::string{"test-fake-input-0"}, 1);
        std::shared_ptr<BufferPool> buffer_pool = std::make_shared<BufferPool>(50, 50);
        std::shared_ptr<ResultPartitionManager> result_partition_manager = std::make_shared<ResultPartitionManager>();

        // Result partition should initialize first, and the input gate follows it.
        ResultPartitionFactory result_partition_factory(result_partition_manager);
        /* Each ResultPartition has 4 Subpartitions */
        std::shared_ptr<ResultPartition> partition_0 = result_partition_factory.create("test-fake-input", 0, 
                                                                                    std::make_shared<ResultPartitionDeploymentDescriptor>(4), buffer_pool);

        InputGateFactory input_gate_factory(result_partition_manager);
        std::shared_ptr<InputGate> input_gate = input_gate_factory.create("test-input-gate-factory-create", 0, input_gate_deployment_descriptor);

        input_gate->request_partitions();


        std::shared_ptr<ResultWriter<Tuple>> result_writer_0 = std::make_shared<ResultWriter<Tuple>>(partition_0, "test");
        // the record cannot load in an entire buffer
        std::shared_ptr<StreamRecord<Tuple>> record_1 = std::make_shared<StreamRecord<Tuple>>(std::make_shared<Tuple2<std::string, int>>(
                                                                                                std::make_shared<std::string>(std::string("hello world")),
                                                                                                std::make_shared<int>(101)));

        result_writer_0->emit(record_1, 0);
        result_writer_0->flush(0);

        std::shared_ptr<StreamTaskNetworkInput<Tuple2<std::string, int>>> input = StreamTaskNetworkInputFactory::create_stream_task_network_input<std::string, int>(input_gate, 0);

        std::shared_ptr<StreamMap<Tuple2<std::string, int>, std::string>> stream_map = std::make_shared<StreamMap<Tuple2<std::string, int>, std::string>>(std::make_shared<StringMapFunction>());

        std::shared_ptr<StreamTaskNetworkOutput<Tuple2<std::string, int>, std::string>> stream_task_network_output = std::make_shared<StreamTaskNetworkOutput<Tuple2<std::string, int>, std::string>>(stream_map);

        /* Test the emit_next() function in stream_network_input  */
        InputStatus input_status = input->emit_next(stream_task_network_output); 
    }
};
