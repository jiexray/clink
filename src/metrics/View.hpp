/**
 * An interface for metrics which should be updated in regular intervals by a background thread
 */
#pragma once

class View
{
public:
    int             UPDATE_INTERVAL_SECONDS = 5;
    virtual void    update() = 0;
};