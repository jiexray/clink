/**
 * Collection of metric names.
 */
#pragma once

#include <string>

class MetricNames
{
public:
    const static std::string SUFFIX_RATE;

    const static std::string IO_NUM_RECORDS_IN;
    const static std::string IO_NUM_RECORDS_OUT;
    const static std::string IO_NUM_RECORDS_IN_RATE;
    const static std::string IO_NUM_RECORDS_OUT_RATE;

    const static std::string IO_NUM_BYTES_IN;
    const static std::string IO_NUM_BYTES_OUT;
    const static std::string IO_NUM_BYTES_IN_RATE;
    const static std::string IO_NUM_BYTES_OUT_RATE;

    const static std::string IO_NUM_BUFFERS_IN;
    const static std::string IO_NUM_BUFFERS_OUT;
    const static std::string IO_NUM_BUFFERS_OUT_RATE;

    const static std::string TASK_IDLE_TIME;
};



