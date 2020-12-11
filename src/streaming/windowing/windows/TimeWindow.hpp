#pragma once
#include "Window.hpp"
#include <string>
#include <iostream>
#include "StringUtils.hpp"

/**
  A Window that represents a time interval from start (inclusive) to end (exclusive).
 */
class TimeWindow: public Window {
private:
    long m_start;
    long m_end;
    
public:
    TimeWindow() {
        m_start = m_end = 0;
    }

    TimeWindow(long start, long end): m_start(start), m_end(end) {
        // std::cout << "create window: " << to_string() << std::endl;
    }

    TimeWindow(const TimeWindow& other) {
        m_start = other.m_start;
        m_end = other.m_end;
        // std::cout << "copy window: " << to_string() << std::endl;
    }

    ~TimeWindow() {
        // std::cout << "destory TimeWindow" << std::endl;
    }

    long get_start() const {
        return m_start;
    }

    long get_end() const {
        return m_end;
    }

    long max_timestamp() const override {
        return m_end - 1;
    }

    bool operator ==(const TimeWindow& other) const {
        return m_start == other.m_start && m_end == other.m_end;
    }

    TimeWindow& operator= (const TimeWindow& other) {
        // std::cout << "TimeWindow assign operator1" << std::endl;
        if (this != (&other)) {
            this->m_start = other.m_start;
            this->m_end = other.m_end;
        }
        return *this;
    }

    TimeWindow& operator= (TimeWindow&& other) {
        // std::cout << "TimeWindow assign operator2" << std::endl;
        if (this != (&other)) {
            this->m_start = other.m_start;
            this->m_end = other.m_end;
        }
        return *this;
    }

    bool operator <(const TimeWindow& other) const {
        return m_start < other.m_start;
    }

    std::string to_string() const {
        return "TimeWindow{start=" + std::to_string(m_start) + ", end=" + std::to_string(m_end) + "}";
    }

    static long get_window_start_with_offset(long timestamp, long offset, long window_size) {
        return timestamp - (timestamp - offset + window_size) % window_size;
    }
};

namespace StringUtils {
    template <>
    inline std::string to_string<TimeWindow>(const TimeWindow& obj) {
        return obj.to_string();
    }
}