#include "TypeSerializerFactory.hpp"

template<>
std::shared_ptr<TypeSerializer<std::string>> TypeSerializerFactory<std::string>::of() {
    return std::make_shared<StringSerializer>();
}

template<>
std::shared_ptr<TypeSerializer<double>> TypeSerializerFactory<double>::of() {
    return std::make_shared<DoubleSerializer>();
}

template<>
std::shared_ptr<TypeSerializer<int>> TypeSerializerFactory<int>::of() {
    return std::make_shared<IntSerializer>();
}

template<>
std::shared_ptr<TypeSerializer<NullType>> TypeSerializerFactory<NullType>::of() {
    throw std::runtime_error("Cannot create serializer for NullType");
}