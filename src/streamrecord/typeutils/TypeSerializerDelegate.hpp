/**
 * A delegate class for TypeSerializer. This class will be used in TupleSerializer, which need to 
 * bind the TypeSerializer's template parameter at runtime.
 */
#pragma once
#include "TypeSerializer.hpp"
#include "TypeSerializerFactory.hpp"
#include <memory>
#include <typeinfo>

enum TypeSerializerDelegateType {
    STRING_TYPE,
    INT_TYPE,
    DOUBLE_TYPE
};

class TypeSerializerDelegate
{
private:
    std::shared_ptr<TypeSerializer<std::string>>        m_string_type_serializer;
    std::shared_ptr<TypeSerializer<double>>             m_double_type_serializer;
    std::shared_ptr<TypeSerializer<int>>                m_int_type_serializer;
    TypeSerializerDelegateType                          m_type;
public:
    TypeSerializerDelegate(const std::type_info& type); 

    StreamRecordAppendResult                            serialize(std::shared_ptr<void> record, 
                                                                std::shared_ptr<BufferBuilder> buffer_builder, 
                                                                bool is_new_record);
};

