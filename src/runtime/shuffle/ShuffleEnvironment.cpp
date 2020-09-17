#include "ShuffleEnvironment.hpp"


std::shared_ptr<spdlog::logger> LocalShuffleEnvironment::m_logger = LoggerFactory::get_logger("LocalShuffleEnvironment");

std::shared_ptr<InputGate>* LocalShuffleEnvironment::create_input_gates(std::string owner_name, InputGateDeploymentDescriptorList & input_gate_descriptors) {
    int number_of_input_gates = (int)input_gate_descriptors.size();
    if (number_of_input_gates == 0) {
        SPDLOG_LOGGER_DEBUG(m_logger, "Task {} do not have InputGate", owner_name);
        return nullptr;
    }
    std::shared_ptr<InputGate>* input_gates = new std::shared_ptr<InputGate>[number_of_input_gates];

    for (size_t i = 0; i < input_gate_descriptors.size(); i++) {
        input_gates[i] = m_input_gate_factory->create(owner_name, i, input_gate_descriptors[i]);
    }
    
    return input_gates;
}

std::shared_ptr<ResultPartition>* LocalShuffleEnvironment::create_result_partitions(std::string owner_name, 
                                                                ResultPartitionDeploymentDescriptorList  result_partition_descriptors,
                                                                std::shared_ptr<BufferPool> buffer_pool) {
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
    return result_partitions;
}