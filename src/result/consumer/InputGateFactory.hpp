/**
 * Factory for InputGate.
 */

#pragma once

#include "InputGate.hpp"
#include "InputGateDeploymentDescriptor.hpp"
#include "ResultPartitionManager.hpp"
#include "InputGate.hpp"
#include "InputChannel.hpp"
#include "InputChannelMetrics.hpp"


class InputGateFactory
{
private:
    typedef std::shared_ptr<InputChannelMetrics> InputChannelMetricsPtr;
    std::shared_ptr<ResultPartitionManager>     m_partition_manager;
    void                                        create_input_channels(std::string owning_task_name, 
                                                                      std::shared_ptr<InputGateDeploymentDescriptor> igdd, 
                                                                      std::shared_ptr<InputGate> input_gate,
                                                                      InputChannelMetricsPtr metrics);

    std::shared_ptr<InputChannel>               create_input_channel(std::shared_ptr<InputGate> input_gate, 
                                                                     int input_channel_idx, 
                                                                     std::string partition_id,
                                                                     InputChannelMetricsPtr metrics);
public:
    InputGateFactory(std::shared_ptr<ResultPartitionManager> partition_manager):
    m_partition_manager(partition_manager) {}

    /**
      Just for test
     */
    std::shared_ptr<InputGate>                  create(std::string owning_task_name, int gate_idx, std::shared_ptr<InputGateDeploymentDescriptor> igdd) {
        throw std::runtime_error("InputGateFactory::create() need InputChannelMetrics, this API has deprecated");
    }

    std::shared_ptr<InputGate>                  create(std::string owning_task_name, 
                                                       int gate_idx, 
                                                       std::shared_ptr<InputGateDeploymentDescriptor> igdd,
                                                       InputChannelMetricsPtr metrics);
};

