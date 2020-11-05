#include "IntValue.hpp"

IntValue::IntValue() {
    m_value = nullptr;
    m_len = 0;
}

IntValue::IntValue(int v) {
    m_value = new int[1];
    memcpy(m_value, &v, sizeof(int));
}

void IntValue::read(std::shared_ptr<TypeDeserializer> deserializer) {
    if (m_value != nullptr) {
        delete[] m_value;
        m_value = nullptr;
    }

    m_value = new int[1];

    m_value[0] = deserializer->read_int();
}

std::string IntValue::to_string() {
    if (m_value == nullptr) {
        return "EMPTY_VALUE";
    }
    return std::to_string(m_value[0]);
}

int IntValue::get_value() {
    if (m_value == nullptr) {
        throw std::runtime_error("Cannot read an empty IntValue");
    }
    return *m_value;
}
