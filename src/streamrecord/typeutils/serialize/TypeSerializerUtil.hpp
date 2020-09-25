/**
 * A delegate class for TypeSerializer. This class will be used in TupleSerializer, which need to 
 * bind the TypeSerializer's template parameter at runtime.
 */
#pragma once
#include "TypeSerializer.hpp"
#include "StringSerializer.hpp"
#include "DoubleSerializer.hpp"
#include "IntSerializer.hpp"
#include <memory>
#include <typeinfo>


class TypeSerializerUtil
{
public:
    static std::shared_ptr<TypeSerializerBase> create_type_basic_type_serializer(const std::type_info& type) {
        if (type == typeid(std::string)) {
            return std::make_shared<StringSerializer>();
        } else if (type == typeid(int)) {
            return std::make_shared<IntSerializer>();
        } else if (type == typeid(double)) {
            return std::make_shared<DoubleSerializer>();
        } else {
            throw std::runtime_error("Unsupport type of serializer");
        }
    }
};

// inline TypeSerializerDelegate::TypeSerializerDelegate(const std::type_info& type) {
//     if (type == typeid(std::string)) {
//         m_string_type_serializer = std::make_shared<StringSerializer>();
//         // m_string_type_serializer = TypeSerializerFactory<std::string>::of();
//         m_type = TypeSerializerDelegateType::STRING_TYPE;
//     } else if (type == typeid(int)) {
//         // m_int_type_serializer = TypeSerializerFactory<int>::of();
//         m_int_type_serializer = std::make_shared<IntSerializer>();
//         m_type = TypeSerializerDelegateType::INT_TYPE;
//     } else if (type == typeid(double)) {
//         // m_double_type_serializer = TypeSerializerFactory<double>::of();
//         m_double_type_serializer = std::make_shared<DoubleSerializer>();
//         m_type = TypeSerializerDelegateType::DOUBLE_TYPE;
//     } else {
//         throw std::runtime_error("Unsupport type of serializer");
//     }
// }

// inline StreamRecordAppendResult TypeSerializerDelegate::serialize(std::shared_ptr<void> record, std::shared_ptr<BufferBuilder> buffer_builder, bool is_new_record) {
//     switch (m_type)
//     {
//     case TypeSerializerDelegateType::STRING_TYPE:
//         return m_string_type_serializer->serialize(std::static_pointer_cast<std::string>(record), buffer_builder, is_new_record);
//     case TypeSerializerDelegateType::INT_TYPE:
//         return m_int_type_serializer->serialize(std::static_pointer_cast<int>(record), buffer_builder, is_new_record);
//     case TypeSerializerDelegateType::DOUBLE_TYPE:
//         return m_double_type_serializer->serialize(std::static_pointer_cast<double>(record), buffer_builder, is_new_record);
//     default:
//         throw std::runtime_error("Unknown type of record");
//     }
// }