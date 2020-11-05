/**
 * IntValue encapsulate the basic functionality of an int, in a serializable and immutable way.
 * TODO: Implement in mutable way
 */
#pragma once
#include "IOReadableWritable.hpp"
#include <string>
#include <cstring>

class IntValue : public IOReadableWritable
{
private:
    int*                    m_value; // character value of the string value
    int                     m_len;

public:
    IntValue();
    IntValue(int v);

    void                    read(std::shared_ptr<TypeDeserializer> deserializer) override;

    /* Properties */
    std::string             to_string();
    int                     get_value();
};
