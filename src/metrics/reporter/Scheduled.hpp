/**
 * Interface for reporters that actively send out data periodly.
 */
#pragma once

class Scheduled
{
public:
    /**
       Report the current measurements. This method is called periodically by the
       metrics registry that uses the reporter.
     */
    virtual void report() = 0;
};

