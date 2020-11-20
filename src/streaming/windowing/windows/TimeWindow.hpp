#pragma once
#include "Window.hpp"
#include <string>

/**
  A Window that represents a time interval from start (inclusive) to end (exclusive).
 */
class TimeWindow: public Window {
private:
    long m_start;
    long m_end;
    
public:
    TimeWindow(long start, long end): m_start(start), m_end(end) {}

    long get_start() {
        return m_start;
    }

    long get_end() {
        return m_end;
    }

    long max_timestamp() override {
        return m_end -1;
    }

    bool operator ==(const TimeWindow& other) const {
        return m_start == other.m_start && m_end == other.m_end;
    }

    std::string to_string() {
        return "TimeWindow{start=" + std::to_string(m_start) + ", end=" + std::to_string(m_end) + "}";
    }

    static long get_window_start_with_offset(long timestamp, long offset, long window_size) {
        return timestamp - (timestamp - offset + window_size) % window_size;
    }
};

