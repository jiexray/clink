/**
 * Util class for string.
 */
#pragma once
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <iterator>

namespace StringUtils {

class StringUtils
{
private:
    
public:
    static std::vector<std::string> split(const std::string& s, char delimiter) {
        std::vector<std::string> tokens;
        std::string token;
        std::istringstream token_stream(s);
        while(std::getline(token_stream, token, delimiter)) {
            tokens.push_back(token);
        }
        return tokens;
    }
};

class StringBuilder {
private:
    std::string m_main;
    std::string m_scratch;

    const std::string::size_type m_scratch_size = 1024;

public:
    StringBuilder& append(const std::string& str) {
        m_scratch.append(str);
        if (m_scratch.size() > m_scratch_size) {
            m_main.append(m_scratch);
            m_scratch.resize(0);
        }
        return *this;
    }

    const std::string& str() {
        if (m_scratch.size() > 0) {
            m_main.append(m_scratch);
            m_scratch.resize(0);
        }
        return m_main;
    }
};

template <class T>
std::string to_string(T value) {
    return value.to_string();
}

template<>
std::string to_string<std::string>(std::string value) {
    return value;
}

template <>
std::string to_string<int>(int value) {
    return std::to_string(value);
}

template <>
std::string to_string<double>(double value) {
    return std::to_string(value);
}

template <class T>
std::string vec_to_string(const T& values) {
    std::string str = "";
    typedef typename T::value_type VEC_T;
    for (int i = 0; i < values.size(); i++) {
        if (i == 0) {
            str += "{";
        }
        str += to_string<VEC_T>(values[i]);

        if (i == values.size() - 1) {
            str += "}";
        } else {
            str += ",";
        }
    }
    return str;
}
}

