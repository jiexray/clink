#pragma once

#include "cxxtest/TestSuite.h"
#include "Tuple2.hpp"
// #include "TypeSerializerDelegate.hpp"
#include "TupleSerializer.hpp"
#include <memory>
#include "TupleDeserializer.hpp"
#include "TupleDeserializationDelegate.hpp"
#include "StreamRecordDeserializer.hpp"


void isBufferEqualToString(std::shared_ptr<BufferBase> buf, std::string str) {
    char char_in_buffer;
    int ret;
    for (int i = 0; i < str.length(); i++) {
        ret = buf->get(&char_in_buffer, i);
        TS_ASSERT_EQUALS(ret == -1, false);
        TS_ASSERT_EQUALS(char_in_buffer, str[i]);
    }
}

class TestStreamRecord : public CxxTest::TestSuite
{
public:
    void testTupleCreate(void) {
        std::shared_ptr<Tuple2<std::string, int>> tuple2 = std::make_shared<Tuple2<std::string, int>>(std::make_shared<std::string>("Hello world"), std::make_shared<int>(100));

        TS_ASSERT_EQUALS(*(tuple2->f0), "Hello world");
        TS_ASSERT_EQUALS(*(tuple2->f1), 100);

        TS_ASSERT_EQUALS(tuple2->get_arity(), 2);

        TS_ASSERT_EQUALS(tuple2->get_field(0) == typeid(std::string), true);
        TS_ASSERT_EQUALS(tuple2->get_field(1) == typeid(int), true);

        std::shared_ptr<Tuple2<std::string, int>> tuple = std::make_shared<Tuple2<std::string, int>>(std::make_shared<std::string>("Hello world"), std::make_shared<int>(100));

        TS_ASSERT_EQUALS(tuple->get_arity(), 2);
        TS_ASSERT_EQUALS(tuple->get_field(0) == typeid(std::string), true);
        TS_ASSERT_EQUALS(tuple->get_field(1) == typeid(int), true);
        TS_ASSERT_EQUALS(tuple->get_field(1) == typeid(std::string), false);
    }

    // void testTypeSerializerDelegate( void ) {
    //     std::shared_ptr<TypeSerializerDelegate> serializer_delegate = std::make_shared<TypeSerializerDelegate>(typeid(double));

    //     std::shared_ptr<StreamRecord<double>> double_record_1 = std::make_shared<StreamRecord<double>>(std::make_shared<double>(12.34), 10);
    //     std::shared_ptr<BufferPool> buffer_pool = std::make_shared<BufferPool>(10, 10);
    //     std::shared_ptr<BufferBuilder> buffer_builder_1 = buffer_pool->request_buffer_builder();

    //     StreamRecordAppendResult serialize_result_1 = serializer_delegate->serialize(double_record_1->get_value(), buffer_builder_1, true);
    //     TS_ASSERT_EQUALS(serialize_result_1, FULL_RECORD_BUFFER_FULL);

    //     std::shared_ptr<BufferBase> buf = buffer_builder_1->create_buffer_consumer()->build();
    //     char* int_buf = new char[2];
    //     SerializeUtils::serialize_short(int_buf, 8);   
    //     double val = 12.34;
    //     char* double_buf = (char*)(&val);
    //     char result_buf[11];
    //     memset(result_buf, 0, 11);
    //     memcpy(result_buf, int_buf, 2);
    //     memcpy(result_buf + 2, double_buf, 8);
    //     std::string buf_1 = std::string(result_buf, 10);
        
    //     isBufferEqualToString(buf, buf_1);
    // }

    void testTupleSerializer( void ) {
        std::cout << "test testTupleSerializer()" << std::endl;
        std::shared_ptr<TupleSerializer<Tuple2<std::string, int>>> tuple_serializer = std::make_shared<TupleSerializer<Tuple2<std::string, int>>>();
        std::shared_ptr<Tuple2<std::string, int>> tuple = std::make_shared<Tuple2<std::string, int>>(std::make_shared<std::string>("Hello world"), std::make_shared<int>(100));

        std::shared_ptr<BufferPool> buffer_pool = std::make_shared<BufferPool>(10, 100);
        std::shared_ptr<BufferBuilder> buffer_builder_1 = buffer_pool->request_buffer_builder();
        std::shared_ptr<BufferConsumer> buffer_consumer_1 = buffer_builder_1->create_buffer_consumer();
        tuple_serializer->serialize(tuple, buffer_builder_1, true);

        std::shared_ptr<BufferBase> buf_1 = buffer_consumer_1->build();
        int buf_size_1 = buf_1->get_max_capacity();

        char* int_buf_total_len = new char[2];
        SerializeUtils::serialize_short(int_buf_total_len, 11 + 4 + 2 + 2);
        std::string buf_total_len_part = std::string(int_buf_total_len, 2);

        char* int_buf_str = new char[2];
        SerializeUtils::serialize_short(int_buf_str, 11);
        std::string buf_string_part= std::string(int_buf_str, 2) + "Hello world";

        char* int_buf_int = new char[2];
        SerializeUtils::serialize_short(int_buf_int, 4);   
        int val = 100;
        char* int_value_buf = (char*)(&val);
        char result_buf[7];
        memset(result_buf, 0, 7);
        memcpy(result_buf, int_buf_int, 2);
        memcpy(result_buf + 2, int_value_buf, 4);
        std::string buf_int_part = std::string(result_buf, 6);

        std::string buf_final = buf_total_len_part + buf_string_part + buf_int_part;

        TS_ASSERT_EQUALS(buf_size_1, buf_final.size());

        for (int i = 0; i < buf_size_1; i++) {
            char c;
            buf_1->get(&c, i);
            TS_ASSERT_EQUALS(c, buf_final[i]);
        }
    } 

    void testTupleSerializerWithSplit( void ) {
        std::cout << "test testTupleSerializerWithSplit()" << std::endl;
        std::shared_ptr<TupleSerializer<Tuple2<std::string, int>>> tuple_serializer = std::make_shared<TupleSerializer<Tuple2<std::string, int>>>();
        std::shared_ptr<Tuple2<std::string, int>> tuple = std::make_shared<Tuple2<std::string, int>>(std::make_shared<std::string>("Hello world"), std::make_shared<int>(100));

        std::shared_ptr<BufferPool> buffer_pool = std::make_shared<BufferPool>(10, 15);
        std::shared_ptr<BufferBuilder> buffer_builder_1 = buffer_pool->request_buffer_builder();
        std::shared_ptr<BufferBuilder> buffer_builder_2 = buffer_pool->request_buffer_builder();
        StreamRecordAppendResult result = tuple_serializer->serialize(tuple, buffer_builder_1, true);
        TS_ASSERT_EQUALS(result, StreamRecordAppendResult::PARTITAL_RECORD_BUFFER_FULL);
        std::shared_ptr<BufferBase> buf_1 = buffer_builder_1->create_buffer_consumer()->build();

        result = tuple_serializer->serialize(tuple, buffer_builder_2, false);
        TS_ASSERT_EQUALS(result, StreamRecordAppendResult::FULL_RECORD);
        std::shared_ptr<BufferBase> buf_2 = buffer_builder_2->create_buffer_consumer()->build();

        char* int_buf_total_len = new char[2];
        SerializeUtils::serialize_short(int_buf_total_len, 11 + 4 + 2 + 2);
        std::string buf_total_len_part = std::string(int_buf_total_len, 2);

        char* int_buf_str = new char[2];
        SerializeUtils::serialize_short(int_buf_str, 11);
        std::string buf_string_part= std::string(int_buf_str, 2) + "Hello world";

        char* int_buf_int = new char[2];
        SerializeUtils::serialize_short(int_buf_int, 4);   
        int val = 100;
        char* int_value_buf = (char*)(&val);
        char result_buf[7];
        memset(result_buf, 0, 7);
        memcpy(result_buf, int_buf_int, 2);
        memcpy(result_buf + 2, int_value_buf, 4);
        std::string buf_int_part = std::string(result_buf, 6);

        std::string buf_final = buf_string_part + buf_int_part;

        int idx = 0;

        for (int i = 0; i < buf_total_len_part.size(); i++) {
            char c;
            if (idx < buf_1->get_max_capacity()) {
                buf_1->get(&c, idx++);
                TS_ASSERT_EQUALS(buf_total_len_part[i], c);
            } else {
                buf_2->get(&c, idx - buf_1->get_max_capacity());
                idx++;
                TS_ASSERT_EQUALS(buf_total_len_part[i], c);
            }
        }

        for (int i = 0; i < buf_string_part.size(); i++) {
            char c;
            if (idx < buf_1->get_max_capacity()) {
                buf_1->get(&c, idx++);
                TS_ASSERT_EQUALS(buf_string_part[i], c);
            } else {
                buf_2->get(&c, idx - buf_1->get_max_capacity());
                idx++;
                TS_ASSERT_EQUALS(buf_string_part[i], c);
            }
        }

        // skip one char if there is only one byte left in the buf_1
        if (idx == buf_1->get_max_capacity() - 1) {
            idx++;
        }

        for (int i = 0; i < buf_int_part.size(); i++) {
            char c;
            if (idx < buf_1->get_max_capacity()) {
                buf_1->get(&c, idx++);
                TS_ASSERT_EQUALS(buf_int_part[i], c);
            } else {
                buf_2->get(&c, idx - buf_1->get_max_capacity());
                idx++;
                TS_ASSERT_EQUALS(buf_int_part[i], c);
            }
        }
    } 

    void testTupleDeserialize( void ) {
        std::cout << "test testTupleDeserialize()" << std::endl;
        // -------------------------
        //  Serialization part
        // -------------------------
        std::shared_ptr<TupleSerializer<Tuple2<std::string, int>>> tuple_serializer = std::make_shared<TupleSerializer<Tuple2<std::string, int>>>();
        // std::shared_ptr<TupleSerializer> tuple_serializer = std::make_shared<TupleSerializer>();
        std::shared_ptr<Tuple2<std::string, int>> tuple = std::make_shared<Tuple2<std::string, int>>(std::make_shared<std::string>("Hello world"), std::make_shared<int>(100));

        std::shared_ptr<BufferPool> buffer_pool = std::make_shared<BufferPool>(10, 100);
        std::shared_ptr<BufferBuilder> buffer_builder_1 = buffer_pool->request_buffer_builder();
        std::shared_ptr<BufferConsumer> buffer_consumer_1 = buffer_builder_1->create_buffer_consumer();
        tuple_serializer->serialize(tuple, buffer_builder_1, true);

        std::shared_ptr<BufferBase> buf_1 = buffer_consumer_1->build();

        // -------------------------
        //  Deserialization part
        // -------------------------

        std::shared_ptr<TupleDeserializer> tuple_deserializer = std::make_shared<TupleDeserializer>();
        tuple_deserializer->set_next_buffer(buf_1);
        buf_1.reset();


        std::shared_ptr<TupleDeserializationDelegate> tuple_deserialize_delegate = std::make_shared<TupleDeserializationDelegate>(2, 
                                                                                new std::reference_wrapper<const std::type_info>[2]{typeid(std::string), typeid(int)});


        tuple_deserializer->get_next_record(tuple_deserialize_delegate);

        std::shared_ptr<Tuple> gen_tuple = tuple_deserialize_delegate->get_instance();

        std::shared_ptr<Tuple2<std::string, int>> gen_real_tuple = std::dynamic_pointer_cast<Tuple2<std::string, int>>(gen_tuple);

        std::cout << "Tuple2 f0: " << *gen_real_tuple->f0 << ", f1: " << *gen_real_tuple->f1 << std::endl;
    }

    void testTupleDeserializeWithStreamDeserializer( void ) {
        std::cout << "test testTupleDeserializeWithStreamDeserializer()" << std::endl;
        // -------------------------
        //  Serialization part
        // -------------------------
        // std::shared_ptr<TupleSerializer> tuple_serializer = std::make_shared<TupleSerializer>();
        std::shared_ptr<TupleSerializer<Tuple2<std::string, int>>> tuple_serializer = std::make_shared<TupleSerializer<Tuple2<std::string, int>>>();
        std::shared_ptr<Tuple2<std::string, int>> tuple = std::make_shared<Tuple2<std::string, int>>(std::make_shared<std::string>("Hello world"), std::make_shared<int>(100));

        std::shared_ptr<BufferPool> buffer_pool = std::make_shared<BufferPool>(10, 100);
        std::shared_ptr<BufferBuilder> buffer_builder_1 = buffer_pool->request_buffer_builder();
        std::shared_ptr<BufferConsumer> buffer_consumer_1 = buffer_builder_1->create_buffer_consumer();
        tuple_serializer->serialize(tuple, buffer_builder_1, true);

        std::shared_ptr<BufferBase> buf_1 = buffer_consumer_1->build();

        // -------------------------
        //  Deserialization part
        // -------------------------
        std::shared_ptr<StreamRecordDeserializer> stream_record_deserializer = std::make_shared<StreamRecordDeserializer>(RECORD_TYPE::TUPLE);

        std::shared_ptr<IOReadableWritable> tuple_deserialize_delegate = std::make_shared<TupleDeserializationDelegate>(2, 
                                                                            new std::reference_wrapper<const std::type_info>[2]{typeid(std::string), typeid(int)});

        stream_record_deserializer->set_next_buffer(buf_1);

        stream_record_deserializer->get_next_record(tuple_deserialize_delegate);

        std::shared_ptr<Tuple> gen_tuple = std::dynamic_pointer_cast<TupleDeserializationDelegate>(tuple_deserialize_delegate)->get_instance();

        std::shared_ptr<Tuple2<std::string, int>> gen_real_tuple = std::dynamic_pointer_cast<Tuple2<std::string, int>>(gen_tuple);

        std::cout << "Tuple2 f0: " << *gen_real_tuple->f0 << ", f1: " << *gen_real_tuple->f1 << std::endl;
    }
};