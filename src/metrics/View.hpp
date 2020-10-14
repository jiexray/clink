/**
 * An interface for metrics which should be updated in regular intervals by a background thread
 */
#pragma once
#include <boost/asio.hpp>

class View
{
public:
    static int      UPDATE_INTERVAL_SECONDS;
    static boost::posix_time::seconds UPDATE_INTERVAL_SECONDS_INTERVAL;
    virtual void    update() = 0;
};