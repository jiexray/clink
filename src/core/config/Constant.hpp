/**
 * Constant values
 */
#pragma once
#include <unistd.h>
#include <spdlog/spdlog.h>
#include <string>


class Constant
{
public:
    /* Utils */
    static int                                  BUFFER_SIZE;
    static std::string                          CLINK_BASE;

    static std::string                          SPDLOG_PATTERN;
    static std::string                          get_log_file_name() {return "logs/taskexecutor-" + std::to_string(getpid()) + ".txt";}
    static spdlog::level::level_enum            SPDLOG_LEVEL;
    static bool                                 SPDLOG_WRITE_FILE;
};
