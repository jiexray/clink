/**
 * a properties class with added utility method to extract primitives
 */
#pragma once
#include <string>
#include <map>

// TODO: implement metric configuation
class MetricConfig
{
public:
    const static std::string MERTICS_REPORTER_INTERVAL;

    int get_integer(const std::string& key, int default_value) const {
        // TODO
        return default_value;
    }

    long get_long(const std::string& key, long default_value) const {
        // TODO
    }

    double get_double(const std::string& key, double default_value) const {

    }

    std::string get_string(const std::string& key, std::string default_value) {

    }

    bool get_bool(const std::string& key, bool default_value) {
        
    }
};
