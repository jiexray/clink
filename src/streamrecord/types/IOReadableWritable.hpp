/**
 * This interface must be implemented by every class whose objects have to be serialized to their binary representation
 * and vice-versa.
 */
#pragma once
#include "TypeDeserializer.hpp"
#include <memory>


class TypeDeserializer;

class IOReadableWritable
{
private:
public:
    virtual ~IOReadableWritable() {}

    virtual void                                    read(TypeDeserializer* deserializer) = 0;
    virtual std::shared_ptr<IOReadableWritable>     get_field(int pos) {return nullptr;}
    virtual std::shared_ptr<void>                   get_instance_void() {return nullptr;}
};

