#include "DeserializationDelegate.hpp"

template<>
void DeserializationDelegate<int>::read(TypeDeserializer* deserializer){
    std::cout << "DeserializationDelegate<int>::read()" << std::endl;
    this->m_real_value = std::make_shared<IntValue>();
    this->m_real_value->read(deserializer);
}

template<>
std::shared_ptr<int> DeserializationDelegate<int>::get_instance() {
    std::shared_ptr<IntValue> int_real_value = std::dynamic_pointer_cast<IntValue>(m_real_value);
    return std::make_shared<int>(int_real_value->get_value());
}

template<>
std::shared_ptr<double> DeserializationDelegate<double>::get_instance() {
    std::shared_ptr<DoubleValue> int_real_value = std::dynamic_pointer_cast<DoubleValue>(m_real_value);
    return std::make_shared<double>(int_real_value->get_value());
}

template<>
void DeserializationDelegate<double>::read(TypeDeserializer* deserializer){
    this->m_real_value = std::make_shared<DoubleValue>();
    this->m_real_value->read(deserializer);
}

template<>
std::shared_ptr<std::string> DeserializationDelegate<std::string>::get_instance() {
    std::shared_ptr<StringValue> int_real_value = std::dynamic_pointer_cast<StringValue>(m_real_value);
    return std::make_shared<std::string>(int_real_value->get_value());
}

template<>
void DeserializationDelegate<std::string>::read(TypeDeserializer* deserializer){
    this->m_real_value = std::make_shared<StringValue>();
    this->m_real_value->read(deserializer);
}