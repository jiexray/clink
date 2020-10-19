#include "LoggerReporter.hpp"

std::shared_ptr<spdlog::logger> LoggerReporter::m_logger = nullptr;
std::string LoggerReporter::line_separator = "\n";

void LoggerReporter::report() {
    StringUtils::StringBuilder builder;

    builder.append(line_separator)
            .append("=========================== Starting metrics report ===========================")
            .append(line_separator);

    builder.append(line_separator)
            .append("-- Counters -------------------------------------------------------------------")
            .append(line_separator);
    std::map<std::string, CounterPtr>::iterator counters_iter = m_counters.begin();
    while (counters_iter != m_counters.end()) {
        builder.append(counters_iter->first).append(": ").append(std::to_string(counters_iter->second->get_count()))
                .append(line_separator);
        ++counters_iter;
    }

    builder.append(line_separator)
            .append("-- Meters ---------------------------------------------------------------------")
            .append(line_separator);
    std::map<std::string, MeterPtr>::iterator meters_iter = m_meters.begin();
    while (meters_iter != m_meters.end()) {
        builder.append(meters_iter->first).append(": ").append(std::to_string(meters_iter->second->get_rate()))
                .append(line_separator);
        ++meters_iter;
    }

    builder.append(line_separator)
            .append("-- Gauges ---------------------------------------------------------------------")
            .append(line_separator);
    std::map<std::string, GaugePtr>::iterator gauges_iter = m_gauges.begin();
    while (gauges_iter != m_gauges.end()){
        std::string s;
        gauges_iter->second->get_value(s);
        builder.append(gauges_iter->first).append(": ").append(s)
                .append(line_separator);
        ++gauges_iter;
    }
    

    builder.append(line_separator)
            .append("=========================== Finished metrics report ===========================")
            .append(line_separator);

    // std::cout << "LoggerReporter::report(): " << builder.str();

    SPDLOG_LOGGER_INFO(m_logger, builder.str());
}