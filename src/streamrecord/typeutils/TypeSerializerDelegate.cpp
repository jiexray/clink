#include "TypeSerializerDelegate.hpp"

TypeSerializerDelegate::TypeSerializerDelegate(const std::type_info& type) {
    if (type == typeid(std::string)) {
        m_string_type_serializer = TypeSerializerFactory<std::string>::of();
        m_type = TypeSerializerDelegateType::STRING_TYPE;
    } else if (type == typeid(int)) {
        m_int_type_serializer = TypeSerializerFactory<int>::of();
        m_type = TypeSerializerDelegateType::INT_TYPE;
    } else if (type == typeid(double)) {
        m_double_type_serializer = TypeSerializerFactory<double>::of();
        m_type = TypeSerializerDelegateType::DOUBLE_TYPE;
    } else {
        throw std::runtime_error("Unsupport type of serializer");
    }
}

StreamRecordAppendResult TypeSerializerDelegate::serialize(std::shared_ptr<void> record, std::shared_ptr<BufferBuilder> buffer_builder, bool is_new_record) {
    switch (m_type)
    {
    case TypeSerializerDelegateType::STRING_TYPE:
        return m_string_type_serializer->serialize(std::static_pointer_cast<std::string>(record), buffer_builder, is_new_record);
    case TypeSerializerDelegateType::INT_TYPE:
        return m_int_type_serializer->serialize(std::static_pointer_cast<int>(record), buffer_builder, is_new_record);
    case TypeSerializerDelegateType::DOUBLE_TYPE:
        return m_double_type_serializer->serialize(std::static_pointer_cast<double>(record), buffer_builder, is_new_record);
    default:
        throw std::runtime_error("Unknown type of record");
    }
}