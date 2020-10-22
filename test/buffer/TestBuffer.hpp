#pragma once

#include "cxxtest/TestSuite.h"
#include "Buffer.hpp"
#include "BufferBuilder.hpp"
#include "BufferConsumer.hpp"
#include "BufferPool.hpp"
#include "BufferBase.hpp"
#include "stdio.h"
#include <thread>
#include <chrono>
#include <stdlib.h>
#include <time.h>

/**
 * Thread functions for multi-thread tests.
 */
void bufferPoolRequestAndRecycle(std::shared_ptr<BufferPool> bufferPool) {
    int round = 100;
    srand(time(NULL));

    for(int i = 0; i < round; i++) {
        std::shared_ptr<BufferBuilder> bufferBuilder = bufferPool->request_buffer_builder();
        std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 100));
        if (bufferBuilder != nullptr) {
            bufferPool->recycle(bufferBuilder);
        }
    }
}

void bufferPoolBlockingRequestAndRecycle(std::shared_ptr<BufferPool> bufferPool, int tid) {
    int round = 50;
    srand(time(NULL));

    for(int i = 0; i < round; i++) {
        std::shared_ptr<BufferBuilder> bufferBuilder = bufferPool->request_buffer_builder_blocking();
        std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 100));
        if (bufferBuilder != nullptr) {
            // std::cout << "Thread: [" << tid << "] recycle a buffer" << std::endl;
            bufferPool->recycle(bufferBuilder);
            // wait for next request
            std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 100 + 100));
        } else {
            std::cout << "BUG! using request_buffer_builder_blocking, must return a non-null buffer builder" << std::endl; 
        }
    }
}

class TestBuffer : public CxxTest::TestSuite
{
private:
    /* data */
public:
    void testCreateNewBuffer( void ) {
        Buffer* buffer = new Buffer(100);
        TS_ASSERT_EQUALS(100, buffer->get_max_capacity());
    }

    void testCreateWithExistBuffer( void ) {
        Buffer* buffer = new Buffer(100);
        std::shared_ptr<BufferBase> slice_buffer = buffer->read_only_slice(0, 10);
        TS_ASSERT_EQUALS(10, slice_buffer->get_max_capacity());
    }

    void testGetAndPutBuffer( void ) {
        Buffer* buffer = new Buffer(100);
        buffer->put(0, '1');
        unsigned char c;
        buffer->get(&c, 0);
        TS_ASSERT_EQUALS('1', c);
    }

    void testBufferBuilderAppend( void ) {
        Buffer* buffer = new Buffer(100);
        BufferBuilder* bufferBuilder = new BufferBuilder(buffer);

        unsigned char data[10] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
        bufferBuilder->append(data, 0, 10);
        for(int i = 0; i < 10; i++) {
            unsigned char c;
            buffer->get(&c, i);
            TS_ASSERT_EQUALS(c, '0' + i);
        }
    }

    void testCreateBufferConsumer( void ) {
        Buffer* buffer = new Buffer(100);
        BufferBuilder* bufferBuilder = new BufferBuilder(buffer);
        std::shared_ptr<BufferConsumer> bufferConsumer = bufferBuilder->create_buffer_consumer();

        unsigned char data[10] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
        for (int i = 0; i < 10; i++) {
            bufferBuilder->append(data, i, 1);
            TS_ASSERT_EQUALS(bufferConsumer->get_write_position(), bufferBuilder->get_write_position());
        }
        std::shared_ptr<BufferBase> read_buffer = bufferConsumer->build();
        TS_ASSERT_EQUALS(10, read_buffer->get_max_capacity());
        for (int i = 0; i < 10; i++) {
            unsigned char c;
            read_buffer->get(&c, i);
            TS_ASSERT_EQUALS(c, '0' + i);
        }
    }

    void testCreateBufferPool( void ) {
        BufferPool bufferPool(2, 100);
        std::shared_ptr<BufferBuilder> bufferBuilder = bufferPool.request_buffer_builder();
        std::shared_ptr<BufferConsumer> bufferConsumer = bufferBuilder->create_buffer_consumer();

        unsigned char data[10] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
        for (int i = 0; i < 10; i++) {
            bufferBuilder->append(data, i, 1);
            TS_ASSERT_EQUALS(bufferConsumer->get_write_position(), bufferBuilder->get_write_position());
        }
        std::shared_ptr<BufferBase> read_buffer = bufferConsumer->build();
        TS_ASSERT_EQUALS(10, read_buffer->get_max_capacity());
        for (int i = 0; i < 10; i++) {
            unsigned char c;
            read_buffer->get(&c, i);
            TS_ASSERT_EQUALS(c, '0' + i);
        }
    }

    void testBufferPoolBufferBuilderRequest( void ) {
        BufferPool bufferPool(2, 100);
        int num_available_buffers = bufferPool.get_number_of_available_buffers();
        TS_ASSERT_EQUALS(num_available_buffers, 2);

        std::shared_ptr<BufferBuilder> bufferBuilder_1 = bufferPool.request_buffer_builder();
        std::shared_ptr<BufferConsumer> bufferConsumer_1 = bufferBuilder_1->create_buffer_consumer();

        num_available_buffers = bufferPool.get_number_of_available_buffers();
        TS_ASSERT_EQUALS(num_available_buffers, 1);

        std::shared_ptr<BufferBuilder> bufferBuilder_2 = bufferPool.request_buffer_builder();
        std::shared_ptr<BufferConsumer> bufferConsumer_2 = bufferBuilder_2->create_buffer_consumer();

        num_available_buffers = bufferPool.get_number_of_available_buffers();
        TS_ASSERT_EQUALS(num_available_buffers, 0);

        std::shared_ptr<BufferBuilder> bufferBuilder_3 = bufferPool.request_buffer_builder();
        TS_ASSERT_EQUALS(bufferBuilder_3, nullptr);
    }


    void testBufferConsumerFinished( void ) {
        BufferPool bufferPool(1, 1);

        std::shared_ptr<BufferBuilder> bufferBuilder_1 = bufferPool.request_buffer_builder();
        std::shared_ptr<BufferConsumer> bufferConsumer_1 = bufferBuilder_1->create_buffer_consumer();

        unsigned char c = '1';
        bufferBuilder_1->append(&c, 0, 1);
        TS_ASSERT_EQUALS(false, bufferConsumer_1->is_finished());
    }

    void testBufferPoolRequestAndRecycle( void ) {
        BufferPool bufferPool(1, 100);

        std::shared_ptr<BufferBuilder> bufferBuilder_1 = bufferPool.request_buffer_builder();
        std::shared_ptr<BufferConsumer> bufferConsumer_1 = bufferBuilder_1->create_buffer_consumer();

        /* Each buffer consumer can only have one reference count */
        TS_ASSERT_EQUALS(bufferConsumer_1.use_count(), 1);
        TS_ASSERT_EQUALS(bufferBuilder_1.use_count(), 1)

        bufferPool.recycle(bufferBuilder_1);

        TS_ASSERT_EQUALS(bufferConsumer_1.use_count(), 1);
        TS_ASSERT_EQUALS(bufferBuilder_1.use_count(), 1)
    }

    void testBufferPoolMultithreadRequestAndRecycle( void ) {
        return;
        std::shared_ptr<BufferPool> bufferPool = std::make_shared<BufferPool>(1, 100);

        std::thread t1(bufferPoolRequestAndRecycle, bufferPool), t2(bufferPoolRequestAndRecycle, bufferPool), t3(bufferPoolRequestAndRecycle, bufferPool);

        t1.join();
        t2.join();
        t3.join();
    }

    void testBufferPoolBlockingRequestAndRecycle( void ) {
        return;
        std::shared_ptr<BufferPool> bufferPool = std::make_shared<BufferPool>(1, 100);

        std::thread t1(bufferPoolBlockingRequestAndRecycle, bufferPool, 1),
         t2(bufferPoolBlockingRequestAndRecycle, bufferPool, 2), 
         t3(bufferPoolBlockingRequestAndRecycle, bufferPool, 3);

        t1.join();
        t2.join();
        t3.join();
    }

    void testIllegalBufferGetAndPut( void ) {
        Buffer* buffer = new Buffer(100);
        TS_ASSERT_THROWS(buffer->put(101, '1'), std::invalid_argument);
        unsigned char c;
        TS_ASSERT_EQUALS(buffer->get(&c, 110), -1);
    }

    // void testIllegalBufferSlice( void ) {
    //     std::cout << "test testIllegalBufferSlice()" << std::endl;
    //     Buffer* buffer = new Buffer(100);
    //     std::shared_ptr<BufferBase> illegal_slice = buffer->read_only_slice(101,1);
    //     if (illegal_slice != nullptr) {
    //         TS_ASSERT_EQUALS(false, true);
    //         std::cout << "error" << std::endl;
    //     }
    //     // if (illegal_slice == nullptr) {
    //     //     TS_ASSERT_EQUALS(true, true);
    //     // } else {
    //     //     TS_ASSERT_EQUALS(false, true);
    //     // }
    //     // TS_ASSERT_EQUALS(illegal_slice.get() == nullptr, true);
    //     // TS_ASSERT_EQUALS(buffer->read_only_slice(99, 3), nullptr);
    //     TS_ASSERT_EQUALS(buffer->read_only_slice(99, 1) == nullptr, false);
    //     TS_ASSERT_EQUALS(buffer->read_only_slice(90, 9) == nullptr, false);
    //     TS_ASSERT_EQUALS(buffer->read_only_slice(90, 4)->read_only_slice(0, 4) == nullptr, false);
    //     TS_ASSERT_THROWS_NOTHING(buffer->read_only_slice(90, 4)->read_only_slice(0, 4));
    //     // TS_ASSERT_THROWS_NOTHING(buffer->read_only_slice(90, 4)->read_only_slice(3, 1));
    //     std::cout << "finish test testIllegalBufferSlice()" << std::endl;
    // }

    void testIllegalBufferBuilderAppend( void ) {
        Buffer* buffer = new Buffer(1);
        BufferBuilder* bufferBuilder = new BufferBuilder(buffer);

        unsigned char data[10] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
        int ret = bufferBuilder->append(data, 0, 1);
        TS_ASSERT_EQUALS(ret, 1)
        ret = bufferBuilder->append(data, 0, 2);
        TS_ASSERT_EQUALS(ret, 0);
    }

    
};


