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
#include "Configuration.hpp"
#include "LoggerFactory.hpp"
#include <memory>
#include <unistd.h>


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

    static std::shared_ptr<spdlog::logger>  m_logger;
public:
    LocalShuffleEnvironment(std::shared_ptr<ResultPartitionManager> result_partition_manager, 
                            std::shared_ptr<ResultPartitionFactory> result_partition_factory,
                            std::shared_ptr<InputGateFactory> input_gate_factory):
    m_result_partition_manager(result_partition_manager),
    m_result_partition_factory(result_partition_factory),
    m_input_gate_factory(input_gate_factory) {
        spdlog::set_pattern(Constant::SPDLOG_PATTERN);
        spdlog::set_level(Constant::SPDLOG_LEVEL);
    }

    std::shared_ptr<InputGate>*             create_input_gates(std::string owner_name, InputGateDeploymentDescriptorList & input_gate_descriptors) override;
    std::shared_ptr<ResultPartition>*       create_result_partitions(std::string owner_name, 
                                                                    ResultPartitionDeploymentDescriptorList  result_partition_descriptors,
                                                                    std::shared_ptr<BufferPool> buffer_pool) override;
};