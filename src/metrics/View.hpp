/**
 * An interface for metrics which should be updated in regular intervals by a background thread
 */
#pragma once

class View
{
public:
    static int      UPDATE_INTERVAL_SECONDS;
    virtual void    update() = 0;
};