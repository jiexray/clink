#include "Constant.hpp"

int                         Constant::BUFFER_SIZE           = 10;
std::string                 Constant::CLINK_BASE            = "/home/tian/software/vscode/projects/clink/";
spdlog::level::level_enum   Constant::SPDLOG_LEVEL          = spdlog::level::debug;
std::string                 Constant::SPDLOG_PATTERN        = "[%T.%e] [%s:%#] [%l] [thread: %t] %v";
bool                        Constant::SPDLOG_WRITE_FILE     = true;
// std::string Configuration::SPDLOG_PATTERN = "[%Y-%m-%d %T.%e] [%s] [%l:%#] %v";