#pragma once

#include <chrono>
#include <cstdlib>
#include <ctime>
#include <sys/time.h>

class TimeUtil
{
public:
    static long current_timestamp() {
        struct timeval tp;
        gettimeofday(&tp, NULL);
        long ms = tp.tv_sec * 1000 + tp.tv_usec / 1000;
        return ms;
    }
};
