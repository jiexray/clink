/**
 * Delagate for tuple deserializtion.
 */
#pragma once
#include <memory>
#include <typeinfo>
#include <vector>
#include <functional>
#include "IOReadableWritable.hpp"
#include "IntValue.hpp"
#include "StringValue.hpp"
#include "DoubleValue.hpp"
#include "TupleUtils.hpp"

class TupleDeserializationDelegate : public IOReadableWritable
{
private:
    /* members for tupleize read */
    int                                     m_num_of_values;
    // (const std::type_info&)*                m_value_types;
    std::shared_ptr<IOReadableWritable>*    m_real_values;

    const std::type_info&                   get_field_type(int pos) {
        if (std::dynamic_pointer_cast<IntValue>(m_real_values[pos]) != nullptr) {
            return typeid(int);
        } else if (std::dynamic_pointer_cast<DoubleValue>(m_real_values[pos]) != nullptr) {
            return typeid(double);
        } else if (std::dynamic_pointer_cast<StringValue>(m_real_values[pos]) != nullptr) {
            return typeid(std::string);
        } else {
            throw std::runtime_error("Unknown type of field value");
        }
    }
public:
    TupleDeserializationDelegate(int num_of_values, std::reference_wrapper<const std::type_info> value_types[]): m_num_of_values(num_of_values){
        m_real_values = new std::shared_ptr<IOReadableWritable>[num_of_values + 1];
        for (int i = 0; i < num_of_values; i++) {
            const std::type_info& field_type = value_types[i];
            if (field_type == typeid(int)) {
                this->m_real_values[i] = std::make_shared<IntValue>();
            } else if (field_type == typeid(double)) {
                this->m_real_values[i] = std::make_shared<DoubleValue>();
            } else if (field_type == typeid(std::string)) {
                this->m_real_values[i] = std::make_shared<StringValue>();
            }
        }
    }

    ~TupleDeserializationDelegate() {
        delete[] m_real_values;
        m_real_values = nullptr;
    }

    /* Properties */
    int                                     get_num_of_values() {return m_num_of_values;}


    /* Implement functions in IOReadableWritable */
    std::shared_ptr<Tuple>                  get_instance();

    void                                    read(TypeDeserializer* deserializer);

    std::shared_ptr<IOReadableWritable>     get_field(int pos);
};

