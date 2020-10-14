/**
 * Util class for string.
 */
#pragma once
#include <string>
#include <vector>
#include <sstream>

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
