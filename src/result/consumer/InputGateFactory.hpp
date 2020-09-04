/**
 * Factory for InputGate.
 */

#pragma once

#include "InputGate.hpp"
#include "../../deployment/InputGateDeploymentDescriptor.hpp"
#include "../ResultPartitionManager.hpp"
#include "InputGate.hpp"
#include "InputChannel.hpp"


class InputGateFactory
{
private:
    std::shared_ptr<ResultPartitionManager>     m_partition_manager;
    void                                        create_input_channels(std::string owning_task_name, std::shared_ptr<InputGateDeploymentDescriptor> igdd, std::shared_ptr<InputGate> input_gate);
    std::shared_ptr<InputChannel>               create_input_channel(std::shared_ptr<InputGate> input_gate, int input_channel_idx, int partition_idx);
public:
    InputGateFactory(std::shared_ptr<ResultPartitionManager> partition_manager):
    m_partition_manager(partition_manager) {}

    std::shared_ptr<InputGate>                  create(std::string owning_task_name, int gate_idx, std::shared_ptr<InputGateDeploymentDescriptor> igdd);
};

