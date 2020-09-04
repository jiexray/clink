/**
 * Identifies ResultSubpartition in a task / sub-task.
 */
#pragma once
#include <string>

class ResultSubpartitionInfo
{
private:
    int m_partition_idx;
    int m_subpartition_idx;
public:
    ResultSubpartitionInfo(int, int);
    ~ResultSubpartitionInfo(){};

    /* Properties */
    int get_partition_idx() {return m_partition_idx;}
    int get_subpartition_idx() {return m_subpartition_idx;} 

    std::string to_string() {return "partition index [" + std::to_string(m_partition_idx) + "], subpartition index [" + std::to_string(m_subpartition_idx) + "]";}
};

inline ResultSubpartitionInfo::ResultSubpartitionInfo(int partition_idx, int subpartition_idx):
m_partition_idx(partition_idx), m_subpartition_idx(subpartition_idx){}

