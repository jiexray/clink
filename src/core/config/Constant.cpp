#include "Constant.hpp"

spdlog::level::level_enum Constant::SPDLOG_LEVEL = spdlog::level::debug;
std::string Constant::SPDLOG_PATTERN = "[%T.%e] [%s:%#] [%l] [thread: %t] %v";
// std::string Configuration::SPDLOG_PATTERN = "[%Y-%m-%d %T.%e] [%s] [%l:%#] %v";