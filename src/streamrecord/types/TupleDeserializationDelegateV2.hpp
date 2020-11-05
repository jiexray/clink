/**
 * Delagate for tuple deserializtion.
 */
#pragma once
#include <memory>
#include <typeinfo>
#include <vector>
#include <functional>
#include <assert.h>
#include "IOReadableWritable.hpp"
#include "IntValue.hpp"
#include "StringValue.hpp"
#include "DoubleValue.hpp"
#include "SerializeUtils.hpp"
#include "TupleUtils.hpp"
#include "DeserializationDelegate.hpp"
#include "TemplateHelper.hpp"

template <class T>
class TupleDeserializationDelegateV2;

template<template <class, class> class T, class T0, class T1>
class TupleDeserializationDelegateV2<T<T0, T1>>: public IOReadableWritable
{
private:
    /* members for tupleize read */
    std::shared_ptr<T0>                     m_f0;
    std::shared_ptr<T1>                     m_f1;

    unsigned char*                          m_tuple_buf;
    unsigned char*                          m_backup_tuple_buf;
public:

    TupleDeserializationDelegateV2() {
        // m_tuple_buf = new unsigned char[65536];
        m_backup_tuple_buf = new unsigned char[65536];
    }

    ~TupleDeserializationDelegateV2() {
        delete[] m_tuple_buf;
        delete[] m_backup_tuple_buf;
    }

    /* Implement functions in IOReadableWritable */
    std::shared_ptr<T<T0, T1>>                  get_instance() {
        return std::make_shared<T<T0, T1>>(this->m_f0, this->m_f1);
    }

    void                                    read(std::shared_ptr<TypeDeserializer> deserializer) {
        throw std::runtime_error("Not Implemented!");
    }

    void                                    read(std::shared_ptr<TypeDeserializer> deserializer, int length) {
        if (length > 65536) {
            throw std::runtime_error("Unsupport tuple size greater than 65536");
        }
        // deserializer->read_unsigned_bytes(m_tuple_buf, length);
        bool to_commit = true;
        deserializer->read_unsigned_bytes_no_copy(&m_tuple_buf, length);

        if (m_tuple_buf == nullptr) {
            to_commit = false;
            m_tuple_buf = m_backup_tuple_buf;
            deserializer->read_unsigned_bytes(m_tuple_buf, length);
        }

        int cur_read_pos = 0;

        // f0 size
        int f0_size = SerializeUtils::deserialize_short(m_tuple_buf + cur_read_pos);
        cur_read_pos += 2;
        // f0 value
        deserialize_value(m_tuple_buf + cur_read_pos, f0_size, Int2Type<0>(), Type2Type<T0>());
        cur_read_pos += f0_size;

        // f1 size
        int f1_size = SerializeUtils::deserialize_short(m_tuple_buf + cur_read_pos);
        cur_read_pos += 2;
        // f1 value
        deserialize_value(m_tuple_buf + cur_read_pos, f1_size, Int2Type<1>(), Type2Type<T1>());
        cur_read_pos += f1_size;

        if (to_commit) {
            deserializer->read_commit();
        }

        assert(cur_read_pos == length);
    }

    void deserialize_value(const unsigned char* buf, int length, Int2Type<0>, Type2Type<std::string>) {
        this->m_f0 = std::make_shared<std::string>((const char*)buf, length);
    }

    void deserialize_value(const unsigned char* buf, int length, Int2Type<0>, Type2Type<int>) {
        this->m_f0 = std::make_shared<int>(SerializeUtils::deserialize_int(buf));
    }

    void deserialize_value(const unsigned char* buf, int length, Int2Type<0>, Type2Type<double>) {
        this->m_f0 = std::make_shared<double>(SerializeUtils::deserialize_double(buf));
    }

    void deserialize_value(const unsigned char* buf, int length, Int2Type<1>, Type2Type<std::string>) {
        this->m_f1 = std::make_shared<std::string>((const char*)buf, length);
    }

    void deserialize_value(const unsigned char* buf, int length, Int2Type<1>, Type2Type<int>) {
        this->m_f1 = std::make_shared<int>(SerializeUtils::deserialize_int(buf));
    }

    void deserialize_value(const unsigned char* buf, int length, Int2Type<1>, Type2Type<double>) {
        this->m_f1 = std::make_shared<double>(SerializeUtils::deserialize_double(buf));
    }
};

