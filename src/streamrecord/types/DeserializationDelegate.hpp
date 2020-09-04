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

template <class T>
class DeserializationDelegate : public IOReadableWritable 
{
private:
    std::shared_ptr<IOReadableWritable>     m_real_value;
public:
    std::shared_ptr<T>                      get_instance();

    void                                    read(TypeDeserializer* deserializer);
};

