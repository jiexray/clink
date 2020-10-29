/**
 * Interface for processing records by StreamTask.
 */
#pragma once
#include "InputStatus.hpp"
#include "AvailabilityProvider.hpp"

class StreamInputProcessor: public AvailabilityProvider
{
public:
    virtual InputStatus process_input() = 0;
    virtual void        close() {}
};

