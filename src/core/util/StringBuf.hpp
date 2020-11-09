/**
 * Container for C string (const char*)
 */
#pragma once

#include <cstring>
#include <iostream>

template <int SIZE>
struct StringBuf
{
    char c_str[SIZE];

    StringBuf() {}
    StringBuf(const char* str) {
        if (strlen(str) >= SIZE) {
            throw std::runtime_error("Buffer in Tuple is insufficient, " 
            + std::to_string(strlen(str)) 
            + " >= " + std::to_string(SIZE));
        }
        strncpy(c_str, str, SIZE);
    }

    StringBuf(const char c) {
        if (SIZE < 2) {
            throw std::runtime_error("Buffer in Tuple is insufficient, " 
            + std::to_string(2) 
            + " > " + std::to_string(SIZE));
        }
        c_str[0] = c;
        c_str[1] = '\0';
    }
};
