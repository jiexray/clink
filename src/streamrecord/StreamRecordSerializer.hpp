/**
 * Serializer for StreamRecord.
 */
#pragma once
#include "TypeSerializer.hpp"
#include "TypeSerializerFactory.hpp"

template<class T>
class StreamRecordSerializer;

template<class T>
class StreamRecordSerializer
{
private:
    std::shared_ptr<TypeSerializer<T>>      m_type_serializer;     
public:
    StreamRecordSerializer() {
        this->m_type_serializer = TypeSerializerFactory<T>::of();
    }

    StreamRecordAppendResult                serialize(std::shared_ptr<StreamRecord<T>> record, 
                                                        std::shared_ptr<BufferBuilder> buffer_bulider,
                                                        bool is_new_record){
        return this->m_type_serializer->serialize(record->get_value(), buffer_bulider, is_new_record);
    }
};


template <template <class, class> class T, class T1, class T2>
class StreamRecordSerializer<T<T1, T2>> {
private:
    std::shared_ptr<TypeSerializer<T<T1, T2>>>      m_type_serializer;
public:
    StreamRecordSerializer() {
        this->m_type_serializer = TypeSerializerFactory<T<T1, T2>>::of();
    }

    StreamRecordAppendResult                serialize(std::shared_ptr<StreamRecord<T<T1, T2>>> record, 
                                                        std::shared_ptr<BufferBuilder> buffer_bulider,
                                                        bool is_new_record){
        return this->m_type_serializer->serialize(record->get_value(), buffer_bulider, is_new_record);
    }
};
