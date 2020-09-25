/**
 * The deserialization delegate is used during deserialization to read an arbitrary element as if it implements
 * IOReadableWritable, with the help of type serializer.
 */
#pragma once
#include <memory>
#include "IOReadableWritable.hpp"
#include "IntValue.hpp"
#include "StringValue.hpp"
#include "DoubleValue.hpp"
#include <typeinfo>

template <class T>
class DeserializationDelegate : public IOReadableWritable 
{
private:
    std::shared_ptr<IOReadableWritable>     m_real_value;
public:
    std::shared_ptr<T>                      get_instance() {
        if ((typeid(T)  == typeid(std::string)) || 
            (typeid(T) == typeid(double)) || 
            (typeid(T) == typeid(int))) {
            return std::static_pointer_cast<T>(m_real_value->get_instance_void());
        } else {
            throw std::invalid_argument("DeserializationDelegate::get_instance() Cannot recoginize the type " + std::string(typeid(T).name()));
        }
    }

    void                                    read(TypeDeserializer* deserializer) {
        if (typeid(T)  == typeid(std::string)) {
            this->m_real_value = std::make_shared<StringValue>();
        } else if (typeid(T) == typeid(double)) {
            this->m_real_value = std::make_shared<DoubleValue>();
        } else if (typeid(T) == typeid(int)) {
            this->m_real_value = std::make_shared<IntValue>();
        } else {
            throw std::invalid_argument("DeserializationDelegate::read() Cannot recoginize the type " + std::string(typeid(T).name()));
        }
        this->m_real_value->read(deserializer);
    }
};

