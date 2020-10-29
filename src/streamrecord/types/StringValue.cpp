#include "StringValue.hpp"

StringValue::StringValue(){
    m_value = nullptr;
    m_len = 0;
}

StringValue::StringValue(std::string s){
    m_len = (int)s.size();
    m_value = new char[m_len + 1];
    memcpy(m_value, s.c_str(), s.size());
}

void StringValue::read(TypeDeserializer* deserializer) {
    m_len = deserializer->get_record_size();
    if (m_value != nullptr) {
        delete[] m_value;
        m_value = nullptr;
    }
    m_value = new char[m_len + 1];

    for (int i = 0; i < m_len; i++) {
        m_value[i] = (char) deserializer->read_unsigned_byte();
    }
}

std::string StringValue::to_string() {
    if (m_value == nullptr) {
        return "EMPTY STRING";
    }
    // std::cout << "StringValue to_string() len: " << m_len << ", value: " << std::string(m_value, m_len) << std::endl;
    return std::string(m_value, m_len);
}

std::string StringValue::get_value() {
    if (m_value == nullptr) {
        throw new std::runtime_error("Cannot read an empty StringValue");
    }
    return std::string(m_value, m_len);
}

std::shared_ptr<void> StringValue::get_instance_void() {
    return std::make_shared<std::string>(std::string(m_value, m_len));
}