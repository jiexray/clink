#pragma once

#include "cxxtest/TestSuite.h"

#include "../../buffer/BufferPool.hpp"
#include "../../buffer/BufferBuilder.hpp"

#include "../../result/ResultPartition.hpp"
#include "../../result/ResultPartitionFactory.hpp"
#include "../../result/ResultSubpartition.hpp"
#include "../../result/ResultPartitionManager.hpp"
#include "../../result/ResultWriter.hpp"
#include "../../result/ChannelSelectorResultWriter.hpp"
#include "../../result/consumer/InputGateFactory.hpp"

#include "../../streamrecord/StreamRecord.hpp"
#include "../../streamrecord/StreamRecordSerializer.hpp"
#include "../../streamrecord/typeutils/StringSerializer.hpp"
#include "../../streamrecord/typeutils/TypeDeserializer.hpp"
#include "../../streamrecord/types/StringValue.hpp"

#include "../../deployment/InputGateDeploymentDescriptor.hpp"
#include "../../deployment/ResultPartitionDeploymentDescriptor.hpp"

#include "../../streaming/operators/SourceOperator.hpp"
#include "../../streaming/operators/StreamMap.hpp"
#include "../../streaming/operators/OneInputStreamOperator.hpp"
#include "../../streaming/operators/StreamOperatorFactory.hpp"
#include "../../streaming/operators/SimpleStreamOperatorFactory.hpp"
#include "../../streaming/operators/SimpleUdfStreamOperatorFactory.hpp"
#include "../../streaming/operators/StreamOperator.hpp"

#include "../../streaming/task/OperatorChain.hpp"
#include "../../streaming/task/StreamTask.hpp"

#include "../../streaming/partitioner/ForwardPartitioner.hpp"

#include "../../streaming/io/ResultWriterOutput.hpp"
#include "../../streaming/io/StreamTaskNetworkInput.hpp"
#include "../../streaming/io/StreamTaskNetworkOutput.hpp"



#include <memory>
#include <cstring>

void isBufferEqualToString(BufferBase* buf, std::string str) {
    char c;
    int ret;
    for (int i = 0; i < str.length(); i++) {
        ret = buf->get(&c, i);
        TS_ASSERT_EQUALS(ret == -1, false);
        TS_ASSERT_EQUALS(c, str[i]);
    }
}

template class MapFunction<std::string, std::string>;
// concat the first char and the last char
class StringMapFunction: public MapFunction<std::string, std::string>{
    std::shared_ptr<std::string> map(std::string& value){
        return std::make_shared<std::string>(value.begin(), value.begin() + 2);
    }
};


class TestOperator: public CxxTest::TestSuite{
public:
    void testOperatorChainCreate() {
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

        std::shared_ptr<StreamTask<std::string>> stream_task = std::make_shared<StreamTask<std::string>>();

        std::shared_ptr<AbstractUdfStreamOperator<Function, std::string>> stream_map = std::make_shared<StreamMap<std::string, std::string>>(std::make_shared<StringMapFunction>());
        // std::shared_ptr<StreamOperator<std::string>> stream_op = std::static_pointer_cast<StreamOperator<std::string>>(stream_map);
        std::shared_ptr<StreamOperatorFactory<std::string>> operator_factory = SimpleStreamOperatorFactory<std::string>::of(stream_map);

        std::shared_ptr<OperatorChain<std::string>> op_chain = std::make_shared<OperatorChain<std::string>>(stream_task, channel_selector_result_writer, operator_factory);
    }

    void testStreamTaskInputWithDataTransferNonBufferSplit( void ) {
        /* Preparation code for InputGate and ResultPartition */

        int* partition_idxs = new int[5] {0};
        /* set consumed_subpartition_index to 0 */
        std::shared_ptr<InputGateDeploymentDescriptor> input_gate_deployment_descriptor = std::make_shared<InputGateDeploymentDescriptor>(0, partition_idxs, 1);
        std::shared_ptr<BufferPool> buffer_pool = std::make_shared<BufferPool>(50, 10);
        std::shared_ptr<ResultPartitionManager> result_partition_manager = std::make_shared<ResultPartitionManager>();

        // Result partition should initialize first, and the input gate follows it.
        ResultPartitionFactory result_partition_factory(result_partition_manager);
        /* Each ResultPartition has 4 Subpartitions */
        std::shared_ptr<ResultPartition> partition_0 = result_partition_factory.create("test-subpartition-add-bufferconsumer-0", 0, 
                                                                                    std::make_shared<ResultPartitionDeploymentDescriptor>(4), buffer_pool);

        InputGateFactory input_gate_factory(result_partition_manager);
        std::shared_ptr<InputGate> input_gate = input_gate_factory.create("test-input-gate-factory-create", 0, input_gate_deployment_descriptor);

        TS_ASSERT_EQUALS(input_gate == nullptr, false);
        input_gate->request_partitions();
        std::map<int, std::shared_ptr<InputChannel>> input_channels = input_gate->get_input_channels();

        std::shared_ptr<InputChannel> input_channel_0 = input_channels[0];


        std::shared_ptr<ResultWriter<std::string>> result_writer_0 = std::make_shared<ResultWriter<std::string>>(partition_0, "test-input-gate-and-result-partition-data-transfer");
        std::shared_ptr<StreamRecord<std::string>> record_1 = std::make_shared<StreamRecord<std::string>>("1234");

        result_writer_0->emit(record_1, 0);
        result_writer_0->flush(0);

        /*   TEST CODE FOR StreamTaskNetworkInput */

        std::shared_ptr<StreamTaskNetworkInput<std::string>> int_stream_network_input = std::make_shared<StreamTaskNetworkInput<std::string>>(input_gate);

        TS_ASSERT_EQUALS(int_stream_network_input == nullptr, false);

        std::shared_ptr<StreamMap<std::string, std::string>> stream_map = std::make_shared<StreamMap<std::string, std::string>>(std::make_shared<StringMapFunction>());

        std::shared_ptr<StreamTaskNetworkOutput<std::string, std::string>> stream_task_network_output = std::make_shared<StreamTaskNetworkOutput<std::string, std::string>>(stream_map);

        /* Test the emit_next() function in stream_network_input  */
        InputStatus input_status = int_stream_network_input->emit_next(stream_task_network_output); 
        TS_ASSERT_EQUALS(input_status, InputStatus::MORE_AVAILABLE);
    } 

    // TODO: testStreamTaskInputWithDataTransferWithBufferSplit
    void testStreamTaskInputWithDataTransferWithBufferSplit( void ) {
        /* Preparation code for InputGate and ResultPartition */

        int* partition_idxs = new int[5] {0};
        /* set consumed_subpartition_index to 0 */
        std::shared_ptr<InputGateDeploymentDescriptor> input_gate_deployment_descriptor = std::make_shared<InputGateDeploymentDescriptor>(0, partition_idxs, 1);
        std::shared_ptr<BufferPool> buffer_pool = std::make_shared<BufferPool>(50, 5);
        std::shared_ptr<ResultPartitionManager> result_partition_manager = std::make_shared<ResultPartitionManager>();

        // Result partition should initialize first, and the input gate follows it.
        ResultPartitionFactory result_partition_factory(result_partition_manager);
        /* Each ResultPartition has 4 Subpartitions */
        std::shared_ptr<ResultPartition> partition_0 = result_partition_factory.create("test-subpartition-add-bufferconsumer-0", 0, 
                                                                                    std::make_shared<ResultPartitionDeploymentDescriptor>(4), buffer_pool);

        InputGateFactory input_gate_factory(result_partition_manager);
        std::shared_ptr<InputGate> input_gate = input_gate_factory.create("test-input-gate-factory-create", 0, input_gate_deployment_descriptor);

        TS_ASSERT_EQUALS(input_gate == nullptr, false);
        input_gate->request_partitions();


        std::shared_ptr<ResultWriter<std::string>> result_writer_0 = std::make_shared<ResultWriter<std::string>>(partition_0, "test");
        // the record cannot load in an entire buffer
        std::shared_ptr<StreamRecord<std::string>> record_1 = std::make_shared<StreamRecord<std::string>>("12345");

        result_writer_0->emit(record_1, 0);
        result_writer_0->flush(0);

        /*   TEST CODE FOR StreamTaskNetworkInput */

        std::shared_ptr<StreamTaskNetworkInput<std::string>> int_stream_network_input = std::make_shared<StreamTaskNetworkInput<std::string>>(input_gate);

        TS_ASSERT_EQUALS(int_stream_network_input == nullptr, false);

        std::shared_ptr<StreamMap<std::string, std::string>> stream_map = std::make_shared<StreamMap<std::string, std::string>>(std::make_shared<StringMapFunction>());

        std::shared_ptr<StreamTaskNetworkOutput<std::string, std::string>> stream_task_network_output = std::make_shared<StreamTaskNetworkOutput<std::string, std::string>>(stream_map);

        /* Test the emit_next() function in stream_network_input  */
        InputStatus input_status = int_stream_network_input->emit_next(stream_task_network_output); 
        TS_ASSERT_EQUALS(input_status, InputStatus::MORE_AVAILABLE);
    }

    void testStreamOperatorCreateWithOperatorFactory(void) {
        std::shared_ptr<AbstractUdfStreamOperator<Function, std::string>> stream_map = std::make_shared<StreamMap<std::string, std::string>>(std::make_shared<StringMapFunction>());
        // std::shared_ptr<StreamOperator<std::string>> stream_op = std::static_pointer_cast<StreamOperator<std::string>>(stream_map);
        std::shared_ptr<StreamOperatorFactory<std::string>> operator_factory = SimpleStreamOperatorFactory<std::string>::of(stream_map);
        // std::shared_ptr<SimpleUdfStreamOperatorFactory<std::string>> udf_operator_factory = std::make_shared<SimpleUdfStreamOperatorFactory<std::string>>(
        //                                 std::static_pointer_cast<AbstractUdfStreamOperator<Function, std::string>>(stream_map));

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

        std::shared_ptr<OperatorChain<std::string>> op_chain = std::make_shared<OperatorChain<std::string>>(stream_task, channel_selector_result_writer, operator_factory);

        std::shared_ptr<StreamOperator<std::string>> stream_op = op_chain->get_head_operator(); 
    }

    void testOperatorDynamicBinding(void) {
        std::shared_ptr<StreamMap<std::string, std::string>> string_map_op = std::make_shared<StreamMap<std::string, std::string>>(std::make_shared<StringMapFunction>());
        std::shared_ptr<StreamOperator<std::string>> str_op = std::make_shared<AbstractUdfStreamOperator<MapFunction<std::string, std::string>, std::string>>(std::make_shared<StringMapFunction>());
        std::shared_ptr<AbstractUdfStreamOperator<StringMapFunction, std::string>> udf_str_op = std::make_shared<AbstractUdfStreamOperator<StringMapFunction, std::string>>(std::make_shared<StringMapFunction>());
        // std::shared_ptr<AbstractUdfStreamOperator<StringMapFunction, std::string>> udf_str_op = 
        std::shared_ptr<AbstractUdfStreamOperator<Function, std::string>> udf_str_op_2 = std::make_shared<StreamMap<std::string, std::string>>(std::make_shared<StringMapFunction>());
        std::shared_ptr<StreamOperator<std::string>> str_op_2 = std::make_shared<StreamMap<std::string, std::string>>(std::make_shared<StringMapFunction>());
        // str_op = udf_str_op_2;
    }
};

