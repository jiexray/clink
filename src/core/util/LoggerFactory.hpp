/**
 * Factory of loggers.
 */
#pragma once
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include "Constant.hpp"

class LoggerFactory
{
public:
    LoggerFactory();
    ~LoggerFactory();

    static std::shared_ptr<spdlog::logger>  get_logger(const std::string& name){
        spdlog::set_pattern(Constant::SPDLOG_PATTERN);
        spdlog::set_level(Constant::SPDLOG_LEVEL);
        std::string logger_name = "taskexecutor-" + std::to_string(getpid());
        if (Constant::SPDLOG_WRITE_FILE) {
            return spdlog::get(logger_name) == nullptr ?
                    spdlog::basic_logger_mt(logger_name, Constant::get_log_file_name()):
                    spdlog::get(logger_name);
        } else {
            return spdlog::get(logger_name) == nullptr ?
                spdlog::stdout_logger_mt(logger_name):
                spdlog::get(logger_name);
        }
    }

    static std::shared_ptr<spdlog::logger> get_logger_with_file_name(const std::string& logger_name, const std::string& file_name) {
        spdlog::set_pattern(Constant::SPDLOG_PATTERN);
        spdlog::set_level(Constant::SPDLOG_LEVEL);
        if (Constant::SPDLOG_WRITE_FILE) {
            return spdlog::get(logger_name) == nullptr ?
                    spdlog::basic_logger_mt(logger_name, "logs/" + file_name + "-" + std::to_string(getpid()) + ".txt"):
                    spdlog::get(logger_name);
        } else {
            return spdlog::get(logger_name) == nullptr ?
                spdlog::stdout_logger_mt(logger_name):
                spdlog::get(logger_name);
        }
    }
};

