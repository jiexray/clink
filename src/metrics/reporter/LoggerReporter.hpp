/**
 * MetricReporter that exports Metrics via Logger. 
 */
#pragma once
#include "Scheduled.hpp"
#include "AbstractMetricReporter.hpp"
#include "LoggerFactory.hpp"
#include "StringUtils.hpp"
#include <memory>

class LoggerReporter: public AbstractMetricReporter, public Scheduled
{
private:
    static std::shared_ptr<spdlog::logger> m_logger;
    static std::string line_separator;
public:

    void open(std::shared_ptr<MetricConfig> config) override {
        m_logger = LoggerFactory::get_logger_with_file_name("LoggerReporter", "metric_report");
    }
    void close() override {
        m_logger->flush();
    }

    void report() override;
};