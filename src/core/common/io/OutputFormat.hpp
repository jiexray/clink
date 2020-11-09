/**
 * The base interface for outputs that consumes records. The output format describe how to 
 * store the final records.
 */
#pragma once
#include <memory>

template <class IT>
class OutputFormat
{
public:
    virtual void                            write_record(IT* record) = 0;
    virtual void                            open(int task_number, int num_tasks) = 0;
    virtual void                            close() = 0;
};
