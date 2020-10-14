#include "MetricNames.hpp"

const std::string MetricNames::SUFFIX_RATE = "PerSecond";
const std::string MetricNames::IO_NUM_RECORDS_IN = "numRecordsIn";
const std::string MetricNames::IO_NUM_RECORDS_OUT = "numRecordsOut";
const std::string MetricNames::IO_NUM_RECORDS_IN_RATE = MetricNames::IO_NUM_RECORDS_IN + MetricNames::SUFFIX_RATE;
const std::string MetricNames::IO_NUM_RECORDS_OUT_RATE = MetricNames::IO_NUM_RECORDS_OUT + MetricNames::SUFFIX_RATE;

const std::string MetricNames::IO_NUM_BYTES_IN = "numBytesIn";
const std::string MetricNames::IO_NUM_BYTES_OUT = "numBytesOut";
const std::string MetricNames::IO_NUM_BYTES_IN_RATE = MetricNames::IO_NUM_BYTES_IN + MetricNames::SUFFIX_RATE;
const std::string MetricNames::IO_NUM_BYTES_OUT_RATE = MetricNames::IO_NUM_BYTES_OUT + MetricNames::SUFFIX_RATE;

const std::string MetricNames::IO_NUM_BUFFERS_IN = "numBuffersIn";
const std::string MetricNames::IO_NUM_BUFFERS_OUT = "numBuffersOut";
const std::string MetricNames::IO_NUM_BUFFERS_OUT_RATE = MetricNames::IO_NUM_BUFFERS_OUT + MetricNames::SUFFIX_RATE;

const std::string MetricNames::TASK_IDLE_TIME = "idleTimeMs" + MetricNames::SUFFIX_RATE;