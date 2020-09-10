/**
 * Interface for the implementation of shuffle service local environment
 */
#pragma once
#include "ResultPartition.hpp"
#include "InputGate.hpp"
#include "ResultPartitionDeploymentDescriptor.hpp"
#include "InputGateDeploymentDescriptor.hpp"
#include "ResultPartitionManager.hpp"
#include "ResultPartitionFactory.hpp"
#include "InputGateFactory.hpp"
#include <memory>

class ShuffleEnvironment
{
public:
    typedef std::vector<std::shared_ptr<ResultPartitionDeploymentDescriptor>> ResultPartitionDeploymentDescriptorList;
    typedef std::vector<std::shared_ptr<InputGateDeploymentDescriptor>> InputGateDeploymentDescriptorList;
    virtual std::shared_ptr<InputGate>*             create_input_gates(std::string owner_name, InputGateDeploymentDescriptorList & input_gate_descriptors) = 0;
    virtual std::shared_ptr<ResultPartition>*       create_result_partitions(std::string owner_name, 
                                                                            ResultPartitionDeploymentDescriptorList  result_partition_descriptors,
                                                                            std::shared_ptr<BufferPool> buffer_pool) = 0;
};

class LocalShuffleEnvironment : public ShuffleEnvironment {
private:
    std::shared_ptr<ResultPartitionManager> m_result_partition_manager;
    std::shared_ptr<ResultPartitionFactory> m_result_partition_factory;
    std::shared_ptr<InputGateFactory>       m_input_gate_factory;
public:
    LocalShuffleEnvironment(std::shared_ptr<ResultPartitionManager> result_partition_manager, 
                            std::shared_ptr<ResultPartitionFactory> result_partition_factory,
                            std::shared_ptr<InputGateFactory> input_gate_factory):
    m_result_partition_manager(result_partition_manager),
    m_result_partition_factory(result_partition_factory),
    m_input_gate_factory(input_gate_factory) {}

    std::shared_ptr<InputGate>*             create_input_gates(std::string owner_name, InputGateDeploymentDescriptorList & input_gate_descriptors) override;
    std::shared_ptr<ResultPartition>*       create_result_partitions(std::string owner_name, 
                                                                    ResultPartitionDeploymentDescriptorList  result_partition_descriptors,
                                                                    std::shared_ptr<BufferPool> buffer_pool) override;
};

inline std::shared_ptr<InputGate>* LocalShuffleEnvironment::create_input_gates(std::string owner_name, InputGateDeploymentDescriptorList & input_gate_descriptors) {
    int number_of_input_gates = (int)input_gate_descriptors.size();
    std::shared_ptr<InputGate>* input_gates = new std::shared_ptr<InputGate>[number_of_input_gates];

    for (size_t i = 0; i < input_gate_descriptors.size(); i++) {
        input_gates[i] = m_input_gate_factory->create(owner_name, i, input_gate_descriptors[i]);
    }
    
    return input_gates;
}

inline std::shared_ptr<ResultPartition>* LocalShuffleEnvironment::create_result_partitions(std::string owner_name, 
                                                                ResultPartitionDeploymentDescriptorList  result_partition_descriptors,
                                                                std::shared_ptr<BufferPool> buffer_pool) {
    std::cout << "LocalShuffleEnvironment::create_result_partitions() task name " << owner_name << ", result partition count " << result_partition_descriptors.size() << std::endl;
    int number_of_result_partitions = (int)result_partition_descriptors.size();
    std::shared_ptr<ResultPartition>* result_partitions = new std::shared_ptr<ResultPartition>[number_of_result_partitions];

    for (size_t i = 0; i < result_partition_descriptors.size(); i++) {
        result_partitions[i] = m_result_partition_factory->create(owner_name, i, result_partition_descriptors[i], buffer_pool);
    }
    return result_partitions;
}