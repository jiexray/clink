/**
 * Interface for processing records by StreamTask.
 */
#pragma once
#include "InputStatus.hpp"

class StreamInputProcessor
{
public:
    virtual InputStatus process_input() = 0;
};

