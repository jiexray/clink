/**
 * This interface must be implemented by every class whose objects have to be serialized to their binary representation
 * and vice-versa.
 */
#pragma once
#include "../typeutils/TypeDeserializer.hpp"
#include <memory>


class TypeDeserializer;

class IOReadableWritable
{
private:
    
public:
    virtual void        read(TypeDeserializer* deserializer) = 0;
};

