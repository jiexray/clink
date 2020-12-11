#pragma once
#include <cstdlib>
#include <string>
#include <sstream>
#include <ctime>

class AbstractID {
private:
    long upper_part;
    long lower_part;

    std::string hex_string = "";

    long lrand()
    {
        if (sizeof(int) < sizeof(long))
            return (static_cast<long>(rand()) << (sizeof(int) * 8)) |
                rand();

        return rand();
    }

public:
    AbstractID() {
        lower_part = lrand();
        upper_part = lrand();
    }

    AbstractID(const AbstractID& other) {
        lower_part = other.lower_part;
        upper_part = other.upper_part;
    }

    long get_lower_part() {
        return lower_part;
    }

    long get_upper_part() {
        return upper_part;
    }

    AbstractID& operator = (const AbstractID& other) {
        if (this != &other) {
            this->lower_part = other.lower_part;
            this->upper_part = other.upper_part;
        }
        return *this;
    }

    AbstractID& operator = (AbstractID&& other) {
        if (this != &other) {
            this->upper_part = other.upper_part;
            this->lower_part = other.lower_part;
        }
        return *this;
    }

    bool operator ==(const AbstractID& other) {
        return lower_part == other.lower_part && upper_part == other.upper_part;
    }

    bool operator <(const AbstractID& other) {
        if (upper_part < other.upper_part) {
            return true;
        } else if (upper_part > other.upper_part) {
            return false;
        } else {
            return lower_part < other.lower_part;
        }
    }

    const std::string& to_string() {
        if (hex_string == "") {
            std::stringstream ss;
            ss << std::hex << upper_part << lower_part;
            hex_string = ss.str();
        } 
        return hex_string;
    }
};
