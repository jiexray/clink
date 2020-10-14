/**
 * Configuration object for MetricRegistryImpl
 */
#pragma once
#include <memory>

class MetricRegistryConfiguration
{
private:
    char                            m_delimiter;

public:
    MetricRegistryConfiguration(char delimiter):
     m_delimiter(delimiter) {}

    /* Properties */
    char                            get_delimiter() {return m_delimiter;}

    /* Static factory method */
    // TODO: config from the Configuration
    static std::shared_ptr<MetricRegistryConfiguration> from_configuration() {
        char delim = '.';

        return std::make_shared<MetricRegistryConfiguration>(delim);
    }
};

