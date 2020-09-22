#include "TupleDeserializationDelegate.hpp"

// -----------------------
//  Tuple read
// -----------------------
std::shared_ptr<Tuple> TupleDeserializationDelegate::get_instance() {
    switch (m_num_of_values) {
    case 2:
        return TupleUtils::create_tuple2_instance(m_real_values[0]->get_instance_void(), m_real_values[1]->get_instance_void(), 
                                                get_field_type(0), get_field_type(1));
    default:
        throw std::runtime_error("Unknown type of Tuple size, currently, only support 2");
    }
}

void TupleDeserializationDelegate::read(TypeDeserializer* deserializer) {
    throw std::runtime_error("No use");
}

std::shared_ptr<IOReadableWritable> TupleDeserializationDelegate::get_field(int pos) {
    if (pos >= m_num_of_values) {
        throw std::invalid_argument("position is out of range, " + std::to_string(pos) + " is out of " + std::to_string(m_num_of_values));
    }
    return this->m_real_values[pos];
}
