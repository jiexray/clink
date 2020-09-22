/**
 * DoubleValue encapsulate the basic functionality of a double, in a serializable and immutable way.
 * TODO: Implement in mutable way
 */
#pragma once
#include "IOReadableWritable.hpp"
#include <string>
#include <cstring>

class DoubleValue : public IOReadableWritable
{
private:
    double*                 m_value; // character value of the string value
    int                     m_len;

public:
    DoubleValue();
    DoubleValue(double v);

    void                    read(TypeDeserializer* deserializer) override;
    std::shared_ptr<void>   get_instance_void() override;

    /* Properties */
    std::string             to_string();
    double                  get_value();
};
