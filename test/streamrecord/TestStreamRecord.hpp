#pragma once

#include "cxxtest/TestSuite.h"
#include "../streamrecord/StreamRecord.hpp"
#include "BufferPool.hpp"
#include "BufferBuilder.hpp"
#include "../streamrecord/StreamRecordSerializer.hpp"
#include "../streamrecord/typeutils/StringSerializer.hpp"
#include <cstring>
#include "../streamrecord/typeutils/TypeDeserializer.hpp"
#include "StringValue.hpp"
#include "DeserializationDelegate.hpp"

#include <memory>

void isBufferEqualToString(BufferBase* buf, std::string str) {
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
    void testStreamRecordCreate() {
        TS_SKIP("skip testStreamRecordCreate");
        StreamRecord<std::string> string_record(std::make_shared<std::string>("12345"), 10);
        std::shared_ptr<StreamRecord<int>> int_record = string_record.replace(std::make_shared<int>(13));

        TS_ASSERT_EQUALS(*int_record->get_value().get(), 13);
        TS_ASSERT_EQUALS(int_record->get_timestamp(), 10);
    }

    void testIntSerializeAndDeserialize() {
        TS_SKIP("skip testIntSerializeAndDeserialize");
        int v = 10;
        char* buf = new char[4];
        SerializeUtils::serialize_int(buf, v);
        TS_ASSERT_EQUALS(v, SerializeUtils::deserialize_int(buf));
    }


    void testStringStreamRecordSerialize( void ) {
        TS_SKIP("skip testStringStreamRecordSerialize");
        std::shared_ptr<StreamRecord<std::string>> string_record_1 = std::make_shared<StreamRecord<std::string>>(std::make_shared<std::string>("1234567"), 10);
        std::shared_ptr<StreamRecord<std::string>> string_record_2 = std::make_shared<StreamRecord<std::string>>(std::make_shared<std::string>("1234567"), 10);
        std::shared_ptr<BufferPool> buffer_pool = std::make_shared<BufferPool>(10, 5);
        std::shared_ptr<BufferBuilder> buffer_builder_1 = buffer_pool->request_buffer_builder();
        std::shared_ptr<BufferBuilder> buffer_builder_2 = buffer_pool->request_buffer_builder();
        std::shared_ptr<BufferBuilder> buffer_builder_3 = buffer_pool->request_buffer_builder();
        std::shared_ptr<BufferBuilder> buffer_builder_4 = buffer_pool->request_buffer_builder();

        StreamRecordSerializer<std::string> stream_record_serializer;

        // only serialize partitial record, length(bitset) + "123"
        StreamRecordAppendResult serialize_result_1 = stream_record_serializer.serialize(string_record_1, buffer_builder_1, true);
        TS_ASSERT_EQUALS(serialize_result_1, PARTITAL_RECORD_BUFFER_FULL);
        StreamRecordAppendResult serialize_result_2 = stream_record_serializer.serialize(string_record_1, buffer_builder_2, false);
        TS_ASSERT_EQUALS(serialize_result_2, FULL_RECORD);
        StreamRecordAppendResult serialize_result_2_2 = stream_record_serializer.serialize(string_record_2, buffer_builder_2, true);
        TS_ASSERT_EQUALS(serialize_result_2_2, NONE_RECORD);
        StreamRecordAppendResult serialize_result_3 = stream_record_serializer.serialize(string_record_2, buffer_builder_3, true);
        TS_ASSERT_EQUALS(serialize_result_3, PARTITAL_RECORD_BUFFER_FULL);

        
        BufferBase* buf = buffer_builder_1->create_buffer_consumer()->build();

        char* int_buf = new char[2];
        SerializeUtils::serialize_short(int_buf, 7);
        std::string buf_1 = std::string(int_buf, 2) + "123";
        isBufferEqualToString(buf, buf_1);

        buf = buffer_builder_2->create_buffer_consumer()->build();
        isBufferEqualToString(buf, "4567\0");

        buf = buffer_builder_3->create_buffer_consumer()->build();
        isBufferEqualToString(buf, buf_1);
    }

    void testDoubleStreamRecordSerializeNonSplit( void ) {
        TS_SKIP("skip testDoubleStreamRecordSerializeNonSplit");
        std::shared_ptr<StreamRecord<double>> double_record_1 = std::make_shared<StreamRecord<double>>(std::make_shared<double>(12.34), 10);
        std::shared_ptr<BufferPool> buffer_pool = std::make_shared<BufferPool>(10, 10);
        std::shared_ptr<BufferBuilder> buffer_builder_1 = buffer_pool->request_buffer_builder();

        StreamRecordSerializer<double> stream_record_serializer;

        StreamRecordAppendResult serialize_result_1 = stream_record_serializer.serialize(double_record_1, buffer_builder_1, true);
        TS_ASSERT_EQUALS(serialize_result_1, FULL_RECORD_BUFFER_FULL);

        BufferBase* buf = buffer_builder_1->create_buffer_consumer()->build();
        char* int_buf = new char[2];
        SerializeUtils::serialize_short(int_buf, 8);   
        double val = 12.34;
        char* double_buf = (char*)(&val);
        char result_buf[11];
        memset(result_buf, 0, 11);
        memcpy(result_buf, int_buf, 2);
        memcpy(result_buf + 2, double_buf, 8);
        std::string buf_1 = std::string(result_buf, 10);
        
        isBufferEqualToString(buf, buf_1);
    }

    void testTypeSerializer(void) {
        TS_SKIP("skip testTypeSerializer");
        std::shared_ptr<StreamRecord<std::string>> string_record_1 = std::make_shared<StreamRecord<std::string>>(std::make_shared<std::string>("1234567"), 10);
        std::shared_ptr<BufferPool> buffer_pool = std::make_shared<BufferPool>(10, 20);
        std::shared_ptr<BufferBuilder> buffer_builder_1 = buffer_pool->request_buffer_builder();

        StreamRecordSerializer<std::string> stream_record_serializer;

        // only serialize partitial record, length(bitset) + "123"
        StreamRecordAppendResult serialize_result_1 = stream_record_serializer.serialize(string_record_1, buffer_builder_1, true);
        TS_ASSERT_EQUALS(serialize_result_1, FULL_RECORD);

        
        BufferBase* buf = buffer_builder_1->create_buffer_consumer()->build();

        char* int_buf = new char[2];
        SerializeUtils::serialize_short(int_buf, 7);
        std::string buf_1 = std::string(int_buf, 2) + "1234567";
        isBufferEqualToString(buf, buf_1);

        TypeDeserializer deserializer;

        deserializer.set_next_buffer(buf);
        std::shared_ptr<StringValue> string_value = std::make_shared<StringValue>();
        deserializer.get_next_record(string_value);
        std::cout << "deserialize_string: " << string_value->to_string() << std::endl;        
    }

    void testDeserializationDelegate( void ) {
        // TS_SKIP("skip testDeserializationDelegate");
        std::shared_ptr<StreamRecord<std::string>> string_record_1 = std::make_shared<StreamRecord<std::string>>(std::make_shared<std::string>("1234567"), 10);
        std::shared_ptr<BufferPool> buffer_pool = std::make_shared<BufferPool>(10, 20);
        std::shared_ptr<BufferBuilder> buffer_builder_1 = buffer_pool->request_buffer_builder();

        StreamRecordSerializer<std::string> stream_record_serializer;

        // only serialize partitial record, length(bitset) + "123"
        StreamRecordAppendResult serialize_result_1 = stream_record_serializer.serialize(string_record_1, buffer_builder_1, true);
        TS_ASSERT_EQUALS(serialize_result_1, FULL_RECORD);

        
        BufferBase* buf = buffer_builder_1->create_buffer_consumer()->build();

        char* int_buf = new char[2];
        SerializeUtils::serialize_short(int_buf, 7);
        std::string buf_1 = std::string(int_buf, 2) + "1234567";
        isBufferEqualToString(buf, buf_1);

        TypeDeserializer deserializer;

        deserializer.set_next_buffer(buf);

        std::shared_ptr<DeserializationDelegate<std::string>> deserialization_delegate = std::make_shared<DeserializationDelegate<std::string>>();
        deserializer.get_next_record(deserialization_delegate);
        std::shared_ptr<std::string> val = deserialization_delegate->get_instance();

        std::cout << "value from deserialization delegate: " << *val.get() << std::endl;
        TS_ASSERT_EQUALS(*val.get(), "1234567");
    }
};

