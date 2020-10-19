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
#include "ShuffleIOOwnerContext.hpp"
#include "ShuffleMetricFactory.hpp"
#include "InputChannelMetrics.hpp"
#include <memory>
#include <unistd.h>


class ShuffleEnvironment
{
public:
    typedef std::vector<std::shared_ptr<ResultPartitionDeploymentDescriptor>> ResultPartitionDeploymentDescriptorList;
    typedef std::vector<std::shared_ptr<InputGateDeploymentDescriptor>> InputGateDeploymentDescriptorList;
    typedef std::shared_ptr<ShuffleIOOwnerContext> ShuffleIOOwnerContextPtr;
    typedef std::shared_ptr<MetricGroup> MetricGroupPtr;
    typedef std::shared_ptr<InputChannelMetrics> InputChannelMetricsPtr;

    /**
      Just for test
     */
    std::shared_ptr<InputGate>*                     create_input_gates(std::string owner_name, InputGateDeploymentDescriptorList & input_gate_descriptors) {
        // Just for test
        throw std::runtime_error("create_input_gates() need ShuffleIOOwnerContext, this API has deprecated");
    }

    /**
      Just for test
     */
    std::shared_ptr<ResultPartition>*               create_result_partitions(std::string owner_name, 
                                                                            ResultPartitionDeploymentDescriptorList  result_partition_descriptors,
                                                                            std::shared_ptr<BufferPool> buffer_pool) {
        // Just for test
        throw std::runtime_error("create_result_partitions() need ShuffleIOOwnerContext, this API has deprecated");
    }

    virtual std::shared_ptr<InputGate>*             create_input_gates(std::string owner_name, 
                                                                       InputGateDeploymentDescriptorList & input_gate_descriptors,
                                                                       ShuffleIOOwnerContextPtr owner_context) = 0;

    virtual std::shared_ptr<ResultPartition>*       create_result_partitions(std::string owner_name, 
                                                                            ResultPartitionDeploymentDescriptorList  result_partition_descriptors,
                                                                            std::shared_ptr<BufferPool> buffer_pool,
                                                                            ShuffleIOOwnerContextPtr owner_context) = 0;
    
    ShuffleIOOwnerContextPtr                        create_shuffle_io_owner_context(const std::string& owner_name, int execution_id, MetricGroupPtr parent_group) {
        MetricGroupPtr shuffle_local_group = ShuffleMetricFactory::create_shuffle_IO_owner_metric_group(parent_group);
        return std::make_shared<ShuffleIOOwnerContext>(
            owner_name,
            execution_id,
            shuffle_local_group,
            shuffle_local_group->add_group(ShuffleMetricFactory::METRIC_GROUP_OUTPUT),
            shuffle_local_group->add_group(ShuffleMetricFactory::METRIC_GROUP_INPUT));
    }
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
    }

    std::shared_ptr<InputGate>*             create_input_gates(std::string owner_name, 
                                                               InputGateDeploymentDescriptorList & input_gate_descriptors, 
                                                               ShuffleIOOwnerContextPtr owner_context) override;
    std::shared_ptr<ResultPartition>*       create_result_partitions(std::string owner_name, 
                                                                    ResultPartitionDeploymentDescriptorList  result_partition_descriptors,
                                                                    std::shared_ptr<BufferPool> buffer_pool,
                                                                    ShuffleIOOwnerContextPtr owner_context) override;
};