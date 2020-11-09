/**
 * Utility class for io.
 */
#pragma once
#include <string>
#include <memory>
#include "Tuple2.hpp"

class IOUtils
{
public:
    template<class T>
    static std::string to_string(T& val) {
        return val.to_string();
    }
};

template <>
inline std::string IOUtils::to_string<double>(double& val) {
    return std::to_string(val);
}

template <>
inline std::string IOUtils::to_string<std::string>(std::string& val) {
    return std::string(val); 
}

template <>
inline std::string IOUtils::to_string<int>(int& val) {
    return std::to_string(val);
}

template <>
inline std::string IOUtils::to_string<const char*>(const char*& val) {
    return std::string(val);
}

// template <template <class, class> class T, class T0, class T1>
// template <T<T0, T1>>
// inline std::string IOUtils::to_string<T<T0, T1>>(std::shared_ptr<T<T0, T1>> val) {
//     return val->to_string();
// }