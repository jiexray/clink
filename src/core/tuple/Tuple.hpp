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

    int                             get_buf_size() {
        int total_size = 0;
        for (int i = 0; i < get_arity(); i++) {
            const std::type_info& field_type = get_field(i);
            
            if (field_type == typeid(int)) {
                total_size += 6; // len(int) + int
            } else if (field_type == typeid(double)) {
                total_size += 10; // len(double) + double
            } else if (field_type == typeid(std::string)) {
                std::shared_ptr<std::string> val = std::static_pointer_cast<std::string>(get_value(i));
                total_size += (2 + (int) val->size()); // len() + string
            } else {
                throw std::runtime_error("Unknown type in Tuple");
            }
        }
        return total_size;
    }

    virtual std::string             to_string() = 0;

    
};
