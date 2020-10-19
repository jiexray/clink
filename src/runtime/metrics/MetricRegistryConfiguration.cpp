#include "MetricRegistryConfiguration.hpp"

std::shared_ptr<MetricRegistryConfiguration> MetricRegistryConfiguration::from_configuration(
        MetricRegistryConfiguration::ConfigurationPtr configuration) {
    char delim = '.';

    return std::make_shared<MetricRegistryConfiguration>(delim);
}