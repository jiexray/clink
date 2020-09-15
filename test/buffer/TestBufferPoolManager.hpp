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

void buffer_write(std::shared_ptr<BufferBuilder> buffer_builder, volatile bool* start_write, volatile bool* stop) {
    while(!(*stop)){
        if (*start_write){
            buffer_builder->append("1", 0, 1);
            std::cout << "start a write" << std::endl;
            *start_write = false;
        }
        // std::cout << "work buffer_write()" << std::endl;
    }
}

void buffer_read(std::shared_ptr<BufferConsumer> buffer_consumer, volatile bool* start_read, volatile bool* stop) {
    while(!(*stop)){
        if (*start_read){
            buffer_consumer->build();
            *start_read = false;
        }
        // std::cout << "work buffer_read()" << std::endl;
    }
}

class TestBufferPoolManager : public CxxTest::TestSuite
{
private:
    
public:
    void testBufferPoolManager() {
        std::cout << "test testBufferPoolManager()" << std::endl;
        std::shared_ptr<BufferPool> buffer_pool = std::make_shared<BufferPool>(10, 100);
        std::shared_ptr<BufferBuilder> buffer_builder_1 = buffer_pool->request_buffer_builder();
        int buffer_id = buffer_builder_1->get_buffer()->get_buffer_id();
        std::shared_ptr<BufferConsumer> buffer_consumer_1 = buffer_builder_1->create_buffer_consumer();


        std::shared_ptr<BufferPoolManager> buffer_pool_mananger = buffer_pool->get_buffer_pool_mananger();

        char data[10] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
        for (int i = 0; i < 10; i++) {
            buffer_builder_1->append(data, i, 1);
        }
        std::shared_ptr<BufferBase> read_buffer = buffer_consumer_1->build();

        TS_ASSERT_EQUALS(buffer_pool_mananger->get_num_consumers(buffer_id), 1);
        TS_ASSERT_EQUALS(buffer_pool_mananger->get_num_slices(buffer_id), 1);
        buffer_consumer_1.reset();
        read_buffer.reset();

        // buffer_id has been evicted from BufferPoolManager
        TS_ASSERT_EQUALS(buffer_pool_mananger->get_num_consumers(buffer_id), -1);
        TS_ASSERT_EQUALS(buffer_pool_mananger->get_num_slices(buffer_id), -1);
    }

    void testMultiThreadBufferReadAndWrite() {
        std::cout << "test testMultiThreadBufferReadAndWrite()" << std::endl;
        std::shared_ptr<BufferPool> buffer_pool = std::make_shared<BufferPool>(1, 1);
        std::shared_ptr<BufferBuilder> buffer_builder = buffer_pool->request_buffer_builder();
        std::shared_ptr<BufferConsumer> buffer_consumer = buffer_builder->create_buffer_consumer();
        volatile bool start_read = false, start_write = false;
        volatile bool stop_read_thread = false, stop_write_thread = false;

        std::thread t_write(buffer_write, buffer_builder, &start_write, &stop_write_thread),
                    t_read(buffer_read, buffer_consumer, &start_read, &stop_read_thread);
        

        // one write;
        start_write = true;
        std::this_thread::sleep_for(std::chrono::seconds(1));

        TS_ASSERT_EQUALS(buffer_consumer->is_finished(), false);


        stop_read_thread = true;
        stop_write_thread = true;

        t_write.join();
        t_read.join();
    }
};

