/**
 * StringValue encapsulate the basic functionality of a String, in a serializable and immutable way.
 * TODO: Implement in mutable way
 */
#pragma once
#include "IOReadableWritable.hpp"
#include <string>
#include <cstring>

class StringValue : public IOReadableWritable
{
private:
    char*                   m_value; // character value of the string value
    int                     m_len;

public:
    StringValue();
    StringValue(std::string s);

    void                    read(TypeDeserializer* deserializer) override;
    std::shared_ptr<void>   get_instance_void() override;

    /* Properties */
    std::string             to_string();
    std::string             get_value();
};
