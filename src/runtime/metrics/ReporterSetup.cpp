#include "ReporterSetup.hpp"

std::vector<ReporterSetup::ReporterSetupPtr> ReporterSetup::from_configuration(ConfigurationPtr configuration) {
    std::vector<ReporterSetupPtr> reporter_setups;

    // TODO: add reporters from configurations
    std::string reporter_name = "logger_reporter";
    MetricConfigPtr metric_config = std::make_shared<MetricConfig>();
    MetricReporterPtr reporter = std::make_shared<LoggerReporter>();

    reporter_setups.push_back(create_reporter_setup(reporter_name, metric_config, reporter));

    return reporter_setups;
}