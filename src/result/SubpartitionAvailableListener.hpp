/**
 * A reader interface for downstream ResultReader to read the data in upstream task's
 * ResultSubpartition.
 */
#pragma once

class SubpartitionAvailableListener
{
public:
    virtual void notify_data_available() = 0;
};
