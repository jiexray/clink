#include "InputGateFactory.hpp"


std::shared_ptr<InputGate> InputGateFactory::create(std::string owning_task_name, 
                                                    int gate_idx, 
                                                    std::shared_ptr<InputGateDeploymentDescriptor> igdd,
                                                    InputChannelMetricsPtr metrics) {
    // TODO: if we support remote network data polling, we need to initialize the buffer pool in the input gate.
    std::shared_ptr<InputGate> input_gate = std::make_shared<InputGate>(owning_task_name, gate_idx, 
                                                                        igdd->get_consumed_subpartition_idx(),
                                                                        (int) igdd->get_input_channels_partition_ids().size());
    
    create_input_channels(owning_task_name, igdd, input_gate, metrics);
    return input_gate;
}

void InputGateFactory::create_input_channels(
        std::string owning_task_name, 
        std::shared_ptr<InputGateDeploymentDescriptor> igdd, 
        std::shared_ptr<InputGate> input_gate,
        InputChannelMetricsPtr metrics) {
    std::vector<std::string> partition_ids = igdd->get_input_channels_partition_ids(); 
    int num_input_channels = (int) partition_ids.size();

    std::shared_ptr<InputChannel>* input_channels = new std::shared_ptr<InputChannel>[num_input_channels];

    for (int i = 0; i < num_input_channels; i++) {
        input_channels[i] = create_input_channel(input_gate, i, partition_ids[i], metrics);
    }

    input_gate->set_input_channels(input_channels, num_input_channels);
}

std::shared_ptr<InputChannel> InputGateFactory::create_input_channel(
        std::shared_ptr<InputGate> input_gate, 
        int input_channel_idx, 
        std::string partition_id,
        InputChannelMetricsPtr metrics) {
    return std::make_shared<InputChannel>(
        input_gate, 
        input_channel_idx, 
        partition_id, 
        m_partition_manager, 
        metrics->get_bytes_in_local(),
        metrics->get_buffers_in_local());
}
