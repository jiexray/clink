#pragma once

#include "cxxtest/TestSuite.h"
#include "../../result/ResultPartition.hpp"
#include "../../result/ResultPartitionFactory.hpp"
#include "../../result/ResultSubpartition.hpp"
#include "../../deployment/ResultPartitionDeploymentDescriptor.hpp"
#include "../../result/ResultPartitionManager.hpp"
#include "../../result/consumer/InputGate.hpp"
#include "../../result/consumer/InputChannel.hpp"
#include "../../result/ResultWriter.hpp"
#include "../../deployment/InputGateDeploymentDescriptor.hpp"
#include "../../result/consumer/InputGateFactory.hpp"
#include "../../result/ChannelSelectorResultWriter.hpp"
#include "../../streaming/partitioner/ForwardPartitioner.hpp"
#include <memory>
#include <string>

void isBufferEqualToString(BufferBase* buf, std::string str) {
    char c;
    int ret;
    for (int i = 0; i < str.length(); i++) {
        ret = buf->get(&c, i);
        TS_ASSERT_EQUALS(ret == -1, false);
        TS_ASSERT_EQUALS(c, str[i]);
    }
}

class TestBuffer : public CxxTest::TestSuite{
public:
    void testCreatePartition( void ) {
        TS_SKIP("skip testCreatePartition");
        std::shared_ptr<BufferPool> buffer_pool = std::make_shared<BufferPool>(10, 100);
        std::shared_ptr<ResultPartitionManager> result_partition_manager = std::make_shared<ResultPartitionManager>();

        ResultPartitionFactory factory(result_partition_manager);
        std::shared_ptr<ResultPartition> partition = factory.create("test-partition-create", 0, std::make_shared<ResultPartitionDeploymentDescriptor>(1), buffer_pool);

        TS_ASSERT_THROWS_NOTHING(partition->get_subpartition(0));
        TS_ASSERT_THROWS(partition->get_subpartition(1), std::invalid_argument);
    }

    void testCreateResultPartitionManager( void ) {
        TS_SKIP("skip testCreateResultPartitionManager");
        std::shared_ptr<BufferPool> buffer_pool = std::make_shared<BufferPool>(10, 100);
        std::shared_ptr<ResultPartitionManager> result_partition_manager = std::make_shared<ResultPartitionManager>();

        ResultPartitionFactory factory(result_partition_manager);
        std::shared_ptr<ResultPartition> partition = factory.create("test-partition-create", 0, std::make_shared<ResultPartitionDeploymentDescriptor>(1), buffer_pool);

        TS_ASSERT_THROWS_NOTHING(result_partition_manager->get_result_partition(0));
        TS_ASSERT_EQUALS(0, result_partition_manager->get_result_partition(0)->get_partition_idx());
        TS_ASSERT_THROWS(result_partition_manager->get_result_partition(1), std::invalid_argument);
        TS_ASSERT_EQUALS(result_partition_manager->get_result_partition(0)->get_subpartition(0)->get_result_subpartition_info()->get_partition_idx(), 0);
        TS_ASSERT_EQUALS(result_partition_manager->get_result_partition(0)->get_subpartition(0)->get_result_subpartition_info()->get_subpartition_idx(), 0);
    }

    void testCreateResultWriter( void ) {
        TS_SKIP("skip testCreateResultWriter");
        std::shared_ptr<BufferPool> buffer_pool = std::make_shared<BufferPool>(10, 100);
        std::shared_ptr<ResultPartitionManager> result_partition_manager = std::make_shared<ResultPartitionManager>();

        ResultPartitionFactory factory(result_partition_manager);
        std::shared_ptr<ResultPartition> partition = factory.create("test-create-result-writer", 0, std::make_shared<ResultPartitionDeploymentDescriptor>(1), buffer_pool);

        std::shared_ptr<ResultWriter<std::string>> result_writer = std::make_shared<ResultWriter<std::string>>(partition, "test-create-result-writer");
    }

    void testResultWriterRequestBufferBuilder( void ) {
        TS_SKIP("skip testResultWriterRequestBufferBuilder");
        std::shared_ptr<BufferPool> buffer_pool = std::make_shared<BufferPool>(10, 100);
        std::shared_ptr<ResultPartitionManager> result_partition_manager = std::make_shared<ResultPartitionManager>();

        ResultPartitionFactory factory(result_partition_manager);
        std::shared_ptr<ResultPartition> partition = factory.create("test-create-result-writer", 0, std::make_shared<ResultPartitionDeploymentDescriptor>(1), buffer_pool);

        std::shared_ptr<ResultWriter<std::string>> result_writer = std::make_shared<ResultWriter<std::string>>(partition, "test-create-result-writer");
        std::shared_ptr<BufferBuilder> buffer_builder = result_writer->get_buffer_builder(0);

        std::shared_ptr<BufferBuilder> buffer_builder_2 = result_writer->get_buffer_builder(0);

        std::shared_ptr<BufferBuilder> buffer_builder_3 = result_writer->request_new_buffer_builder(0);
 
        TS_ASSERT_EQUALS(buffer_builder, buffer_builder_2);
        TS_ASSERT_EQUALS(buffer_builder == buffer_builder_3, false);
    }

    void testResultWriterCopyToBufferBuilder( void ) {
        TS_SKIP("skip testResultWriterCopyToBufferBuilder");
        std::shared_ptr<BufferPool> buffer_pool = std::make_shared<BufferPool>(10, 5);
        std::shared_ptr<ResultPartitionManager> result_partition_manager = std::make_shared<ResultPartitionManager>();

        ResultPartitionFactory factory(result_partition_manager);
        std::shared_ptr<ResultPartition> partition = factory.create("test-create-result-writer", 0, std::make_shared<ResultPartitionDeploymentDescriptor>(4), buffer_pool);

        std::shared_ptr<ResultWriter<std::string>> result_writer = std::make_shared<ResultWriter<std::string>>(partition, "test-create-result-writer");

        std::shared_ptr<StreamRecord<std::string>> record_1 = std::make_shared<StreamRecord<std::string>>("1234");
        std::shared_ptr<StreamRecord<std::string>> record_2 = std::make_shared<StreamRecord<std::string>>("12345");
        std::shared_ptr<StreamRecord<std::string>> record_3 = std::make_shared<StreamRecord<std::string>>("123456");
        std::shared_ptr<StreamRecord<std::string>> record_4 = std::make_shared<StreamRecord<std::string>>("1234");
        std::shared_ptr<StreamRecord<std::string>> record_5 = std::make_shared<StreamRecord<std::string>>("12345");

        result_writer->copy_to_buffer_builder(0, record_1);
        std::shared_ptr<BufferBuilder> buffer_builder = result_writer->get_buffer_builder(0);
        std::shared_ptr<BufferConsumer> buffer_consumer = buffer_builder->create_buffer_consumer();
        BufferBase* buf = buffer_consumer->build();

        for(int i = 0; i < 4; i++) {
            char c;
            buf->get(&c, i);
            TS_ASSERT_EQUALS(c, '1' + i);
        }

        result_writer->copy_to_buffer_builder(1, record_2);
        std::shared_ptr<BufferBuilder> buffer_builder_1 = result_writer->get_buffer_builder(1);
        std::shared_ptr<BufferConsumer> buffer_consumer_1 = buffer_builder_1->create_buffer_consumer();
        buf = buffer_consumer_1->build();
        TS_ASSERT_EQUALS(buf, nullptr); // copy_to_buffer_builder get FULL_RECORD_BUFFER_FULL, release the buffer builder at once

        result_writer->copy_to_buffer_builder(2, record_3);
        std::shared_ptr<BufferBuilder> buffer_builder_2 = result_writer->get_buffer_builder(2);
        std::shared_ptr<BufferConsumer> buffer_consumer_2 = buffer_builder_2->create_buffer_consumer();
        buf = buffer_consumer_2->build();
        
        // "12345" have been written to previous buffer builder, this buffer builder only contain "6"
        for(int i = 0; i < 1; i++) {
            char c;
            buf->get(&c, i);
            TS_ASSERT_EQUALS(c, '6');
        }

        // test add consequtive stream records to buffer builder
        result_writer->copy_to_buffer_builder(3, record_4);
        result_writer->copy_to_buffer_builder(3, record_5);
        std::shared_ptr<BufferBuilder> buffer_builder_3 = result_writer->get_buffer_builder(3);
        std::shared_ptr<BufferConsumer> buffer_consumer_3 = buffer_builder_3->create_buffer_consumer();
        buf = buffer_consumer_3->build();
        TS_ASSERT_EQUALS(buf == nullptr, false);

        // "1234" and "1" is written to previous buffer builder, this buffer contain "2345"
        for(int i = 0; i < 4; i++) {
            char c;
            buf->get(&c, i);
            TS_ASSERT_EQUALS(c, '2' + i);
        }
    }

    void testInputChannelCreate( void ) {
        TS_SKIP("skip testInputChannelCreate");
        std::shared_ptr<BufferPool> buffer_pool = std::make_shared<BufferPool>(10, 100);
        std::shared_ptr<ResultPartitionManager> result_partition_manager = std::make_shared<ResultPartitionManager>();

        ResultPartitionFactory factory(result_partition_manager);
        std::shared_ptr<ResultPartition> partition = factory.create("test-create-result-writer", 0, std::make_shared<ResultPartitionDeploymentDescriptor>(1), buffer_pool);

        std::shared_ptr<InputGate> fake_input_gate = std::make_shared<InputGate>(0);
        std::shared_ptr<InputChannel> input_channel = std::make_shared<InputChannel>(fake_input_gate, 0, 0, result_partition_manager);

        input_channel->request_subpartition(0);
        std::shared_ptr<ResultSubpartitionView> subpartition_view_in_input_channel = input_channel->get_subpartition_view();
        std::shared_ptr<ResultSubpartitionView> subpartition_view_in_subpartition = partition->get_subpartition(0)->get_read_view();

        TS_ASSERT_EQUALS(subpartition_view_in_input_channel, subpartition_view_in_subpartition);
    }


    // this test should block the notification response from ResultReader
    void testsubpartitionAddBufferConsumer( void ) {
        // TS_SKIP("skip testsubpartitionAddBufferConsumer");   
        std::shared_ptr<BufferPool> buffer_pool = std::make_shared<BufferPool>(10, 7);
        std::shared_ptr<ResultPartitionManager> result_partition_manager = std::make_shared<ResultPartitionManager>();

        ResultPartitionFactory factory(result_partition_manager);
        std::shared_ptr<ResultPartition> partition = factory.create("test-subpartition-add-bufferconsumer", 0, std::make_shared<ResultPartitionDeploymentDescriptor>(4), buffer_pool);

        std::shared_ptr<InputGate> fake_input_gate = std::make_shared<InputGate>(0);
        std::shared_ptr<InputChannel> input_channel = std::make_shared<InputChannel>(fake_input_gate, 0, 0, result_partition_manager);
        std::shared_ptr<ResultSubpartition> subpartition_0 = partition->get_subpartition(0);
        std::shared_ptr<ResultSubpartitionView> read_view_0 = subpartition_0->create_read_view(input_channel);
        std::shared_ptr<ResultSubpartition> subpartition_1 = partition->get_subpartition(1);
        std::shared_ptr<ResultSubpartition> subpartition_2 = partition->get_subpartition(2);
        std::shared_ptr<ResultSubpartition> subpartition_3 = partition->get_subpartition(3);
        TS_ASSERT_EQUALS(subpartition_0 == nullptr, false);


        std::shared_ptr<ResultWriter<std::string>> result_writer = std::make_shared<ResultWriter<std::string>>(partition, "test-subpartition-add-bufferconsumer");

        TS_ASSERT_EQUALS(subpartition_0->should_notify_data_available(), false);

        std::shared_ptr<StreamRecord<std::string>> record_1 = std::make_shared<StreamRecord<std::string>>("1234");
        std::shared_ptr<StreamRecord<std::string>> record_2 = std::make_shared<StreamRecord<std::string>>("12345");
        std::shared_ptr<StreamRecord<std::string>> record_3 = std::make_shared<StreamRecord<std::string>>("123456");
        std::shared_ptr<StreamRecord<std::string>> record_4 = std::make_shared<StreamRecord<std::string>>("1234");
        std::shared_ptr<StreamRecord<std::string>> record_5 = std::make_shared<StreamRecord<std::string>>("12345");

        result_writer->emit(record_1, 0);
        // subpartition 0 is full and finish
        TS_ASSERT_EQUALS(subpartition_0->should_notify_data_available(), false);
        // Occasionally, subpartition 0 flush trigger
        TS_ASSERT_EQUALS(subpartition_0->get_buffers_in_backlog(), 0);
        // 5 bytes, only 4 used, not finished
        TS_ASSERT_EQUALS(subpartition_0->get_number_of_finished_buffers(), 0);
        subpartition_0->flush();
        TS_ASSERT_EQUALS(subpartition_0->get_flush_requested(), true);
        TS_ASSERT_EQUALS(subpartition_0->get_buffers_in_backlog(), 1);
        TS_ASSERT_EQUALS(subpartition_0->get_number_of_finished_buffers(), 0);

        // actually, the data is not polled, another add come
        result_writer->emit(record_2, 0);
        TS_ASSERT_EQUALS(subpartition_0->get_flush_requested(), true);
        TS_ASSERT_EQUALS(subpartition_0->get_buffers_in_backlog(), 2);
        TS_ASSERT_EQUALS(subpartition_0->get_number_of_finished_buffers(), 1);
        // has already notify flushing, do not notify again
        TS_ASSERT_EQUALS(subpartition_0->should_notify_data_available(), false);

        // poll data from subpartition 0
        std::shared_ptr<BufferAndBacklog> buffer_and_backlog = subpartition_0->poll_buffer();
        TS_ASSERT_EQUALS(subpartition_0->get_flush_requested(), true);
        TS_ASSERT_EQUALS(subpartition_0->get_buffers_in_backlog(), 1);
        TS_ASSERT_EQUALS(subpartition_0->get_number_of_finished_buffers(), 1);
        TS_ASSERT_EQUALS(buffer_and_backlog == nullptr, false);
        BufferBase* buf = buffer_and_backlog->get_buffer();
        isBufferEqualToString(buf, "\0x0\0x41234\0x0");
        TS_ASSERT_EQUALS(buffer_and_backlog->get_data_available(), true);
        TS_ASSERT_EQUALS(buffer_and_backlog->get_buffers_in_backlog(), 1);

        // poll data again, drain the subpartition 0
        buffer_and_backlog = subpartition_0->poll_buffer();
        TS_ASSERT_EQUALS(subpartition_0->get_flush_requested(), false);
        TS_ASSERT_EQUALS(subpartition_0->get_buffers_in_backlog(), 0);
        TS_ASSERT_EQUALS(subpartition_0->get_number_of_finished_buffers(), 0);
        TS_ASSERT_EQUALS(buffer_and_backlog == nullptr, false);
        buf = buffer_and_backlog->get_buffer();
        isBufferEqualToString(buf, "\0x0\0x512345");
        TS_ASSERT_EQUALS(buffer_and_backlog->get_data_available(), false);
        TS_ASSERT_EQUALS(buffer_and_backlog->get_buffers_in_backlog(), 0);


        // add a large data, need two buffer consumers
        result_writer->emit(record_3, 0);
        TS_ASSERT_EQUALS(subpartition_0->get_flush_requested(), false);
        TS_ASSERT_EQUALS(subpartition_0->get_buffers_in_backlog(), 1);
        TS_ASSERT_EQUALS(subpartition_0->get_number_of_finished_buffers(), 1);
        TS_ASSERT_EQUALS(subpartition_0->should_notify_data_available(), true);

        // poll data, sperated in two buffers "\0x0\0x612345" "6"
        buffer_and_backlog = subpartition_0->poll_buffer();
        TS_ASSERT_EQUALS(subpartition_0->get_flush_requested(), false);
        TS_ASSERT_EQUALS(subpartition_0->get_buffers_in_backlog(), 0);
        TS_ASSERT_EQUALS(subpartition_0->get_number_of_finished_buffers(), 0);
        TS_ASSERT_EQUALS(buffer_and_backlog == nullptr, false);
        buf = buffer_and_backlog->get_buffer();
        isBufferEqualToString(buf, "\0x0\0x612345");

        TS_ASSERT_EQUALS(buffer_and_backlog->get_data_available(), false);
        TS_ASSERT_EQUALS(buffer_and_backlog->get_buffers_in_backlog(), 0);

        // poll the left buffer, this buffer is not finished
        buffer_and_backlog = subpartition_0->poll_buffer();
        TS_ASSERT_EQUALS(subpartition_0->get_flush_requested(), false);
        TS_ASSERT_EQUALS(subpartition_0->get_buffers_in_backlog(), 0);
        TS_ASSERT_EQUALS(subpartition_0->get_number_of_finished_buffers(), 0);
        TS_ASSERT_EQUALS(buffer_and_backlog == nullptr, false);
        buf = buffer_and_backlog->get_buffer();

        isBufferEqualToString(buf, "6");
        TS_ASSERT_EQUALS(buffer_and_backlog->get_data_available(), false);
        TS_ASSERT_EQUALS(buffer_and_backlog->get_buffers_in_backlog(), 0);
            // result_writer->emit(record_4, 1);
            // TS_ASSERT_EQUALS(subpartition_1->get_number_of_finished_buffers(), 1);
            // TS_ASSERT_EQUALS(subpartition_1->should_notify_data_available(), true);
    }

    void testInputGateDeploymentDescriptorCreate( void ) {
        TS_SKIP("skip testInputGateDeploymentDescriptorCreate");
        int* partition_idxs = new int[5] {1,3,5,7,8};
        std::shared_ptr<InputGateDeploymentDescriptor> input_gate_deployment_descriptor = std::make_shared<InputGateDeploymentDescriptor>(0, partition_idxs, 5);
        std::vector<int> input_channels_partitions = input_gate_deployment_descriptor->get_input_channels_partitions();
        int consumed_subpartition_idx = input_gate_deployment_descriptor->get_consumed_subpartition_idx();

        TS_ASSERT_EQUALS(consumed_subpartition_idx, 0);
        for (int i = 0; i < 5; i++) {
            TS_ASSERT_EQUALS(input_channels_partitions[i], partition_idxs[i]);
        }
    }

    void testInputGateFactoryCreate( void ) {
        TS_SKIP("skip testInputGateFactoryCreate");
        int* partition_idxs = new int[5] {1,3,5,7,8};
        std::shared_ptr<InputGateDeploymentDescriptor> input_gate_deployment_descriptor = std::make_shared<InputGateDeploymentDescriptor>(0, partition_idxs, 5);

        std::shared_ptr<ResultPartitionManager> result_partition_manager = std::make_shared<ResultPartitionManager>();
        
        InputGateFactory input_gate_factory(result_partition_manager);

        std::shared_ptr<InputGate> input_gate = input_gate_factory.create("test-input-gate-factory-create", 0, input_gate_deployment_descriptor);
        TS_ASSERT_EQUALS(input_gate == nullptr, false);

        std::map<int, std::shared_ptr<InputChannel>> input_channels = input_gate->get_input_channels();

        for (int i = 0; i < 5; i++) {
            int idx = partition_idxs[i];
            std::shared_ptr<InputChannel> input_channel = input_channels[idx];
            TS_ASSERT_EQUALS(input_channel == nullptr, false);
            TS_ASSERT_EQUALS(input_channel->get_channel_idx(), i);
            TS_ASSERT_EQUALS(input_channel->get_partition_idx(), idx);
        }
    }

    void testInputGateChannelQueueAndGet( void ) {
        TS_SKIP("skip testInputGateChannelQueueAndGet");
        int* partition_idxs = new int[5] {1,3,5,7,8};
        std::shared_ptr<InputGateDeploymentDescriptor> input_gate_deployment_descriptor = std::make_shared<InputGateDeploymentDescriptor>(0, partition_idxs, 5);
        std::shared_ptr<BufferPool> buffer_pool = std::make_shared<BufferPool>(10, 5);
        std::shared_ptr<ResultPartitionManager> result_partition_manager = std::make_shared<ResultPartitionManager>();

        ResultPartitionFactory result_partition_factory(result_partition_manager);
        std::shared_ptr<ResultPartition> partition = result_partition_factory.create("test-subpartition-add-bufferconsumer", 0, 
                                                                                    std::make_shared<ResultPartitionDeploymentDescriptor>(4), buffer_pool);

        InputGateFactory input_gate_factory(result_partition_manager);

        std::shared_ptr<InputGate> input_gate = input_gate_factory.create("test-input-gate-factory-create", 0, input_gate_deployment_descriptor);
        TS_ASSERT_EQUALS(input_gate == nullptr, false);

        std::map<int, std::shared_ptr<InputChannel>> input_channels = input_gate->get_input_channels();


        for (int i = 0; i < 5; i++) {
            int idx = partition_idxs[i];
            std::shared_ptr<InputChannel> input_channel = input_channels[idx];
            TS_ASSERT_EQUALS(input_channel == nullptr, false);
            
            // This test is OK, the invoke of this notify_channel_non_empty() 
            // will be called from the inner of an input channel.
            input_gate->notify_channel_non_empty(input_channel);

            std::shared_ptr<InputChannel> process_input_channel = input_gate->get_channel(false);
            TS_ASSERT_EQUALS(process_input_channel, input_channel);
            process_input_channel = input_gate->get_channel(false);
            TS_ASSERT_EQUALS(process_input_channel, nullptr);
        }
    }

    void testInputGateAndResultPartitionsConnect( void ) {
        TS_SKIP("skip testInputGateAndResultPartitionsConnect");
        int* partition_idxs = new int[5] {0, 2};
        /* set consumed_subpartition_index to 2 */
        std::shared_ptr<InputGateDeploymentDescriptor> input_gate_deployment_descriptor = std::make_shared<InputGateDeploymentDescriptor>(2, partition_idxs, 3);
        std::shared_ptr<BufferPool> buffer_pool = std::make_shared<BufferPool>(50, 5);
        std::shared_ptr<ResultPartitionManager> result_partition_manager = std::make_shared<ResultPartitionManager>();

        // Result partition should initialize first, and the input gate follows it.
        ResultPartitionFactory result_partition_factory(result_partition_manager);
        /* Each ResultPartition has 4 Subpartitions */
        std::shared_ptr<ResultPartition> partition_0 = result_partition_factory.create("test-subpartition-add-bufferconsumer-0", 0, 
                                                                                    std::make_shared<ResultPartitionDeploymentDescriptor>(4), buffer_pool);
        std::shared_ptr<ResultPartition> partition_1 = result_partition_factory.create("test-subpartition-add-bufferconsumer-1", 1, 
                                                                                    std::make_shared<ResultPartitionDeploymentDescriptor>(4), buffer_pool);
        std::shared_ptr<ResultPartition> partition_2 = result_partition_factory.create("test-subpartition-add-bufferconsumer-2", 2, 
                                                                                    std::make_shared<ResultPartitionDeploymentDescriptor>(4), buffer_pool);

        InputGateFactory input_gate_factory(result_partition_manager);
        std::shared_ptr<InputGate> input_gate = input_gate_factory.create("test-input-gate-factory-create", 0, input_gate_deployment_descriptor);

        TS_ASSERT_EQUALS(input_gate == nullptr, false);
        input_gate->request_partitions();
        std::map<int, std::shared_ptr<InputChannel>> input_channels = input_gate->get_input_channels();

        for (int i = 0; i < 2; i++) {
            int idx = partition_idxs[i];
            std::shared_ptr<InputChannel> input_channel = input_channels[idx];
            TS_ASSERT_EQUALS(input_channel == nullptr, false);

            std::shared_ptr<ResultSubpartitionView> view = input_channel->get_subpartition_view();

            // the consumed_subpartition_idx of each input channel is 2 (see InputGateDeploymentDescriptor)
            std::shared_ptr<ResultSubpartitionView> view_from_subpartition = result_partition_manager->get_result_partition(idx)->get_subpartition(2)->get_read_view();

            TS_ASSERT_EQUALS(view, view_from_subpartition);
        }
    }

    void testInputGateAndResultPartitionsDataTransfer( void ) {
        TS_SKIP("skip testInputGateAndResultPartitionsDataTransfer");
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
        std::map<int, std::shared_ptr<InputChannel>> input_channels = input_gate->get_input_channels();

        std::shared_ptr<InputChannel> input_channel_0 = input_channels[0];


        std::shared_ptr<ResultWriter<std::string>> result_writer_0 = std::make_shared<ResultWriter<std::string>>(partition_0, "test-input-gate-and-result-partition-data-transfer");
        std::shared_ptr<StreamRecord<std::string>> record_1 = std::make_shared<StreamRecord<std::string>>("1234");
        std::shared_ptr<StreamRecord<std::string>> record_2 = std::make_shared<StreamRecord<std::string>>("12345");
        std::shared_ptr<StreamRecord<std::string>> record_3 = std::make_shared<StreamRecord<std::string>>("123456");
        std::shared_ptr<StreamRecord<std::string>> record_4 = std::make_shared<StreamRecord<std::string>>("1234");
        std::shared_ptr<StreamRecord<std::string>> record_5 = std::make_shared<StreamRecord<std::string>>("12345");

        result_writer_0->emit(record_1, 0);
        result_writer_0->flush(0);

        // the input channel connect to result_partition_0, i.e., the input_channel_0 is current data available!
        std::shared_ptr<InputChannel> available_input_channel = input_gate->get_channel(false);
        TS_ASSERT_EQUALS(available_input_channel == nullptr, false);

        TS_ASSERT_EQUALS(available_input_channel, input_channel_0);

        std::shared_ptr<ResultSubpartitionView> view_0 = input_channel_0->get_subpartition_view();

        // poll buffer from subpartition view
        std::shared_ptr<BufferAndBacklog> buffer_and_backlog = view_0->get_next_buffer();
        TS_ASSERT_EQUALS(buffer_and_backlog == nullptr, false);
        BufferBase* buf = buffer_and_backlog->get_buffer();
        isBufferEqualToString(buf, "1234");
        TS_ASSERT_EQUALS(buffer_and_backlog->get_data_available(), false);
        TS_ASSERT_EQUALS(buffer_and_backlog->get_buffers_in_backlog(), 0);
    }

    void testChannelSelectorResultWriterCreate( void ) {
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
    }
};
