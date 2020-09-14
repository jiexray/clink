#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include <iostream>
#include <spdlog/spdlog.h>

int main(int, char* [])
{

    spdlog::set_level(spdlog::level::debug); // or 
    spdlog::set_level(spdlog::level::trace); 

SPDLOG_TRACE( "Some trace message that will not be evaluated.{} ,{}", 1, 3.23);
SPDLOG_DEBUG("Some debug message to default logger that will be evaluated");
}
