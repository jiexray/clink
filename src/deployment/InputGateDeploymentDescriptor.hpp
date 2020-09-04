/**
 * The deployment information of a input gate. An input gate manage connection to subpartitions from a same
 * upstream task. The distribution of a subpartition (PONITWISE or ALL_TO_ALL) is determined by the StreamPartition
 * of the logical edge between the consequtive job (vertices).
 * 
 * Subpartition requests do not execute when instantiating the input gate. A request will start on the start of a StreamTask,
 * and the request is based on the consumedSubpartitionIdx in this inputGateDeploymentDescriptor. 
 * 
 * Subpartitions in one ResultPartition connect to downstream subtasks with consequtive subtaskIndices, So we only need to store 
 * the consumedSubpartitionIdx, which is a logical index in the corresponding ResultPartition.
 */

#pragma once
#include <vector>

class InputGateDeploymentDescriptor
{
private:
    std::vector<int>            m_partition_idx_of_input_channels;
    int                         m_consumed_subpartition_idx;

public:
    InputGateDeploymentDescriptor(int consumed_subpartition_idx, int* partition_idxs, int num_partition_idxs):
    m_partition_idx_of_input_channels(partition_idxs, partition_idxs + num_partition_idxs),
    m_consumed_subpartition_idx(consumed_subpartition_idx) {}

    /* properties */
    int                         get_consumed_subpartition_idx() {return m_consumed_subpartition_idx;}
    std::vector<int>&           get_input_channels_partitions() {return m_partition_idx_of_input_channels;}
};

