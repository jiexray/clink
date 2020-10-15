/**
 * Util class for string.
 */
#pragma once
#include <string>
#include <vector>
#include <sstream>

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

}


