/**
 * Serializer for StreamRecord.
 */
#pragma once
#include "typeutils/TypeSerializer.hpp"
#include "typeutils/TypeSerializerFactory.hpp"

template<class T>
class StreamRecordSerializer
{
private:
    std::shared_ptr<TypeSerializer<T>>      m_type_serializer;     
public:
    StreamRecordSerializer();

    StreamRecordAppendResult                serialize(std::shared_ptr<StreamRecord<T>> record, 
                                                        std::shared_ptr<BufferBuilder> buffer_bulider,
                                                        bool is_new_record){
        return this->m_type_serializer->serialize(record->get_value(), buffer_bulider, is_new_record);
    }
};

template<class T>
StreamRecordSerializer<T>::StreamRecordSerializer() {
    // if (typeid(&val) == typeid(std::string)){
    //     this->m_type_serializer = std::make_shared<StringSerializer>();
    // } else if (typeid(&val) == typeid(double)) {
    //     this->m_type_serializer = std::make_shared<DoubleSerializer>();
    // }
    this->m_type_serializer = TypeSerializerFactory<T>::of();
}