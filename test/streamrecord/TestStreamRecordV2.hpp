#pragma once

#include "cxxtest/TestSuite.h"
#include "StreamRecordV2.hpp"
#include "StreamRecordV2Serializer.hpp"
#include "StreamRecordV2Deserializer.hpp"
#include "Tuple2V2.hpp"
#include "BufferPool.hpp"
#include "BufferBuilder.hpp"
#include <iostream>

class TestStreamRecordV2 : public CxxTest::TestSuite {
public:
    void testStreamRecordCreate( void ) {
        StreamRecordV2<int> int_record(10);
        std::cout << "int record: " << int_record.val << std::endl;

        StreamRecordV2<double> double_record(10.091);
        std::cout << "double record: " << double_record.val << std::endl;

        StreamRecordV2<const char*, 32> str_record("hello world");
        std::cout << "string record: " << std::string(str_record.val) << std::endl;

        StreamRecordV2<const char[32]> str_length_fixed("hello world");
        std::cout << "fix string record: " << std::string(str_length_fixed.val) << std::endl;


        int v = 10;
        double d = 10.01;
        const char* ch = "hello everyone";
        StreamRecordV2<Tuple2V2<int, double>> tuple_record(Tuple2V2<int, double>(&v, &d));
        std::cout << "tuple f0: " << tuple_record.val.f0 << ", f1: " << tuple_record.val.f1 << std::endl;

        StreamRecordV2<Tuple2V2<int, const char*, sizeof(int), 32>> tuple_record_f1_string(Tuple2V2<int, const char*, sizeof(int), 32>(&v, ch));
        std::cout << "tuple f0: " << tuple_record_f1_string.val.f0 << ", f1 (str): " << std::string(tuple_record_f1_string.val.f1) << std::endl;
    }

    void testStreamRecordSerializeAndDeserialize( void ) {
        std::shared_ptr<BufferPool> buffer_pool = std::make_shared<BufferPool>(10, 1000);
        std::shared_ptr<BufferBuilder> buffer_builder_1 = buffer_pool->request_buffer_builder();

        std::cout << "test testStreamRecordSerializeAndDeserialize" << std::endl;
        StreamRecordV2<int> int_record(10);

        std::shared_ptr<StreamRecordV2Serializer<StreamRecordV2<int>>> int_record_serializer = std::make_shared<StreamRecordV2Serializer<StreamRecordV2<int>>>();
        int_record_serializer->serialize(int_record, buffer_builder_1, true);

        std::shared_ptr<StreamRecordV2Deserializer<StreamRecordV2<int>>> int_record_deserializer = std::make_shared<StreamRecordV2Deserializer<StreamRecordV2<int>>>();

        std::shared_ptr<BufferBase> buf = buffer_builder_1->create_buffer_consumer()->build();
        int_record_deserializer->set_next_buffer(buf);
        DeserializationResult deserialize_result = int_record_deserializer->get_next_record();
        TS_ASSERT_EQUALS(deserialize_result, DeserializationResult::LAST_RECORD_FROM_BUFFER);


        StreamRecordV2<int>* deserialize_record = int_record_deserializer->get_instance();

        TS_ASSERT_EQUALS(deserialize_record->val, 10);
    }

    void testStreamRecordSerializeAndDeserializeTuple( void ) {
        std::cout << "test testStreamRecordSerializeAndDeserializeTuple()" << std::endl;
        std::shared_ptr<BufferPool> buffer_pool = std::make_shared<BufferPool>(10, 1000);
        std::shared_ptr<BufferBuilder> buffer_builder_1 = buffer_pool->request_buffer_builder();

        int v = 10;
        double d = 10.01;
        const char* ch = "hello everyone";
        StreamRecordV2<Tuple2V2<int, double>> tuple_record(Tuple2V2<int, double>(&v, &d));

        std::shared_ptr<StreamRecordV2Serializer<StreamRecordV2<Tuple2V2<int, double>>>> tuple_record_serializer = std::make_shared<StreamRecordV2Serializer<StreamRecordV2<Tuple2V2<int, double>>>>();
        tuple_record_serializer->serialize(tuple_record, buffer_builder_1, true);

        std::shared_ptr<StreamRecordV2Deserializer<StreamRecordV2<Tuple2V2<int, double>>>> tuple_record_deserializer = std::make_shared<StreamRecordV2Deserializer<StreamRecordV2<Tuple2V2<int, double>>>>();

        std::shared_ptr<BufferBase> buf = buffer_builder_1->create_buffer_consumer()->build();
        tuple_record_deserializer->set_next_buffer(buf);
        DeserializationResult deserialize_result = tuple_record_deserializer->get_next_record();
        TS_ASSERT_EQUALS(deserialize_result, DeserializationResult::LAST_RECORD_FROM_BUFFER);


        StreamRecordV2<Tuple2V2<int, double>>* deserialize_record = tuple_record_deserializer->get_instance();

        TS_ASSERT_EQUALS(deserialize_record->val.f0, 10);
        TS_ASSERT_EQUALS(deserialize_record->val.f1, 10.01);
    }

    void testStreamRecordSerializeAndDeserializeTupleString( void ) {
        std::cout << "test testStreamRecordSerializeAndDeserializeTupleString()" << std::endl;
        std::shared_ptr<BufferPool> buffer_pool = std::make_shared<BufferPool>(10, 1000);
        std::shared_ptr<BufferBuilder> buffer_builder_1 = buffer_pool->request_buffer_builder();

        int v = 10;
        double d = 10.01;
        const char* ch = "hello everyone";
        StreamRecordV2<Tuple2V2<int, const char*, sizeof(int), 32>> tuple_record(Tuple2V2<int, const char*, sizeof(int), 32>(&v, ch));

        std::shared_ptr<StreamRecordV2Serializer<StreamRecordV2<Tuple2V2<int, const char*, sizeof(int), 32>>>> tuple_record_serializer = 
            std::make_shared<StreamRecordV2Serializer<StreamRecordV2<Tuple2V2<int, const char*, sizeof(int), 32>>>>();
        tuple_record_serializer->serialize(tuple_record, buffer_builder_1, true);

        std::shared_ptr<StreamRecordV2Deserializer<StreamRecordV2<Tuple2V2<int, const char*, sizeof(int), 32>>>> tuple_record_deserializer = 
            std::make_shared<StreamRecordV2Deserializer<StreamRecordV2<Tuple2V2<int, const char*, sizeof(int), 32>>>>();

        std::shared_ptr<BufferBase> buf = buffer_builder_1->create_buffer_consumer()->build();
        tuple_record_deserializer->set_next_buffer(buf);
        DeserializationResult deserialize_result = tuple_record_deserializer->get_next_record();
        TS_ASSERT_EQUALS(deserialize_result, DeserializationResult::LAST_RECORD_FROM_BUFFER);


        StreamRecordV2<Tuple2V2<int, const char*, sizeof(int), 32>>* deserialize_record = tuple_record_deserializer->get_instance();

        TS_ASSERT_EQUALS(deserialize_record->val.f0, 10);
        TS_ASSERT_EQUALS(deserialize_record->val.f1, "hello everyone");
    }
};