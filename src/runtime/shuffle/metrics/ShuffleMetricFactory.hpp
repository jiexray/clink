/**
 * Factory for netty shuffle service metrics.
 */
#pragma once
#include "ShuffleIOOwnerContext.hpp"
#include <memory>
#include "InputGate.hpp"
#include "ResultPartition.hpp"

class ShuffleMetricFactory
{
private:
    typedef std::shared_ptr<ShuffleIOOwnerContext> ShuffleIOOwnerContextPtr;
    typedef std::shared_ptr<MetricGroup> MetricGroupPtr;
    typedef std::shared_ptr<InputGate> InputGatePtr;
    typedef std::shared_ptr<ResultPartition> ResultPartitionPtr;

    static void                     register_input_metrics(
            MetricGroupPtr input_group,
            MetricGroupPtr buffers_group,
            InputGatePtr* input_gates, 
            int num_of_input_gates) {
        // TODO: add buffer metric for InputGate
    }

    static void                     register_output_metrics(
            MetricGroupPtr parent_group,
            MetricGroupPtr buffers_group,
            ResultPartitionPtr* result_partitions,
            int num_of_result_partitions) {
        // TODO: add buffer metrics for ResultPartition
    }
public:
    const static std::string        METRIC_GROUP_SHUFFLE;
    const static std::string        METRIC_GROUP_LOCAL;
    const static std::string        METRIC_GROUP_OUTPUT;
    const static std::string        METRIC_GROUP_INPUT;
    const static std::string        METRIC_GROUP_BUFFERS;

    static MetricGroupPtr create_shuffle_IO_owner_metric_group(MetricGroupPtr parent_group) {
        return parent_group->add_group(METRIC_GROUP_SHUFFLE)->add_group(METRIC_GROUP_LOCAL);
    }

    static void                     register_input_metrics(
            MetricGroupPtr input_group, 
            InputGatePtr* input_gates, 
            int num_of_input_gates) {
        register_input_metrics(input_group, input_group->add_group(METRIC_GROUP_BUFFERS), input_gates, num_of_input_gates);
    }

    static void                     register_output_metrics(
            MetricGroupPtr parent_group,
            ResultPartitionPtr* result_partitions,
            int num_of_result_partitions) {
        
    }
};
