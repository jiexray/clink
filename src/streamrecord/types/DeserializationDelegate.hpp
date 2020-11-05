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
#include "TemplateHelper.hpp"

template <class T>
class DeserializationDelegate : public IOReadableWritable 
{
private:
    std::shared_ptr<IOReadableWritable>     m_real_value;

    std::shared_ptr<T>                      get_instance(Type2Type<std::string>) {
        assert(std::dynamic_pointer_cast<StringValue>(m_real_value) != nullptr);
        return std::make_shared<T>(std::dynamic_pointer_cast<StringValue>(m_real_value)->get_value());
    }

    void                                    read(std::shared_ptr<TypeDeserializer> deserializer, Type2Type<std::string>) {
        this->m_real_value = std::make_shared<StringValue>();
        this->m_real_value->read(deserializer);
    }

    std::shared_ptr<T>                      get_instance(Type2Type<int>) {
        assert(std::dynamic_pointer_cast<IntValue>(m_real_value) != nullptr);
        return std::make_shared<T>(std::dynamic_pointer_cast<IntValue>(m_real_value)->get_value());
    }

    void                                    read(std::shared_ptr<TypeDeserializer> deserializer, Type2Type<int>) {
        this->m_real_value = std::make_shared<IntValue>();
        this->m_real_value->read(deserializer);
    }

    std::shared_ptr<T>                      get_instance(Type2Type<double>) {
        assert(std::dynamic_pointer_cast<DoubleValue>(m_real_value) != nullptr);
        return std::make_shared<T>(std::dynamic_pointer_cast<DoubleValue>(m_real_value)->get_value());
    }

    void                                    read(std::shared_ptr<TypeDeserializer> deserializer, Type2Type<double>) {
        this->m_real_value = std::make_shared<DoubleValue>();
        this->m_real_value->read(deserializer);
    }

public:
    std::shared_ptr<T>                      get_instance() {
        return get_instance(Type2Type<T>());
    }

    void                                    read(std::shared_ptr<TypeDeserializer> deserializer) {
        read(deserializer, Type2Type<T>());
    }
};

