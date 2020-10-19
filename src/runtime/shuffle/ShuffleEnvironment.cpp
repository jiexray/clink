#include "ShuffleEnvironment.hpp"


std::shared_ptr<spdlog::logger> LocalShuffleEnvironment::m_logger = LoggerFactory::get_logger("LocalShuffleEnvironment");

std::shared_ptr<InputGate>* LocalShuffleEnvironment::create_input_gates(std::string owner_name, 
                                                                        InputGateDeploymentDescriptorList & input_gate_descriptors,
                                                                        ShuffleIOOwnerContextPtr owner_context) {
    int number_of_input_gates = (int)input_gate_descriptors.size();
    if (number_of_input_gates == 0) {
        SPDLOG_LOGGER_DEBUG(m_logger, "Task {} do not have InputGate", owner_name);
        return nullptr;
    }
    std::shared_ptr<InputGate>* input_gates = new std::shared_ptr<InputGate>[number_of_input_gates];

    MetricGroupPtr network_input_group = owner_context->get_input_group();
    InputChannelMetricsPtr input_channel_metrics = std::make_shared<InputChannelMetrics>(network_input_group);


    for (size_t i = 0; i < input_gate_descriptors.size(); i++) {
        input_gates[i] = m_input_gate_factory->create(owner_name, i, input_gate_descriptors[i], input_channel_metrics);
    }

    // register other metrics on InputGate
    ShuffleMetricFactory::register_input_metrics(network_input_group, input_gates, number_of_input_gates);
    
    return input_gates;
}

std::shared_ptr<ResultPartition>* LocalShuffleEnvironment::create_result_partitions(std::string owner_name, 
                                                                ResultPartitionDeploymentDescriptorList  result_partition_descriptors,
                                                                std::shared_ptr<BufferPool> buffer_pool,
                                                                ShuffleIOOwnerContextPtr owner_context) {
    int number_of_result_partitions = (int)result_partition_descriptors.size();
    if (number_of_result_partitions == 0) {
        SPDLOG_LOGGER_DEBUG(m_logger, "Task {} do not have ResultPartition", owner_name);
        // std::cout << "[DEBUG] Task " << owner_name << " do not have result partitions" << std::endl;
        return nullptr;
    }
    std::shared_ptr<ResultPartition>* result_partitions = new std::shared_ptr<ResultPartition>[number_of_result_partitions];

    for (size_t i = 0; i < result_partition_descriptors.size(); i++) {
        result_partitions[i] = m_result_partition_factory->create(owner_name, i, result_partition_descriptors[i], buffer_pool);
    }

    // register other metrics on ResultPartitions
    ShuffleMetricFactory::register_output_metrics(owner_context->get_output_group(), result_partitions, number_of_result_partitions);

    return result_partitions;
}