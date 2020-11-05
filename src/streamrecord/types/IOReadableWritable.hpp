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

    virtual void                                    read(std::shared_ptr<TypeDeserializer> deserializer) = 0;
    virtual void                                    read(std::shared_ptr<TypeDeserializer> deserializer, int length) {throw std::runtime_error("Not Implemented!");}
    virtual std::shared_ptr<IOReadableWritable>     get_field(int pos) {return nullptr;}
    virtual int                                     get_num_of_values() {return 1;}
};

