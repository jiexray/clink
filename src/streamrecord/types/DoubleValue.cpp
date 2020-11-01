#include "DoubleValue.hpp"

DoubleValue::DoubleValue() {
    m_value = nullptr;
    m_len = 0;
}

DoubleValue::DoubleValue(double v) {
    m_len = sizeof(double);
    m_value = new double[1];
    memcpy(m_value, &v, sizeof(double));
}

void DoubleValue::read(TypeDeserializer* deserializer) {
    if (m_value != nullptr) {
        delete[] m_value;
        m_value = nullptr;
    }
    m_value = new double[1];
    m_value[0] = deserializer->read_double();
}

std::string DoubleValue::to_string() {
    if (m_value == nullptr) {
        return "EMPTY VALUE";
    }
    return std::to_string(m_value[0]);
}

double DoubleValue::get_value() {
    if (m_value == nullptr) {
        throw std::runtime_error("Cannot read an empty DoubleValue");
    }
    return *m_value;
}

std::shared_ptr<void> DoubleValue::get_instance_void(){
    return std::make_shared<double>(*m_value);
}