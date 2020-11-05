/**
 * The base class of all tuples. Tuples have a fix length and contain a set of fields, 
 * which may all be different types. Because Tuples are strongly typed, each distinct
 * tuple length is represented by its own class.
 */
#pragma once
#include <typeinfo>
#include <memory>
#include <string>

class Tuple
{
public:
    virtual int                     get_arity() = 0;
    virtual const std::type_info&   get_field(int pos) = 0;
    virtual std::shared_ptr<void>   get_value(int pos) = 0;

    virtual int                     get_buf_size() = 0;


    virtual std::string             to_string() = 0;

    
};
