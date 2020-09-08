#pragma once
#include "cxxtest/TestSuite.h"
#include <memory>
#include "Task.hpp"

template class MapFunction<std::string, std::string>;
// concat the first char and the last char
class StringMapFunction: public MapFunction<std::string, std::string>{
    std::shared_ptr<std::string> map(std::string& value){
        return std::make_shared<std::string>(value.begin(), value.begin() + 2);
    }

    char* serialize() override {return (char*)this;}

    std::shared_ptr<MapFunction<std::string, std::string>> deserialize() override { return std::make_shared<StringMapFunction>();}
};

class TestTask: public CxxTest::TestSuite{
public:
    void testTaskInit( void ) {
        // std::shared_ptr<Task> task = std::make_shared<Task>();
        std::shared_ptr<JobInformation> job_information = std::make_shared<JobInformation>(0, "test-job");

        std::shared_ptr<Configuration> task_configuration = std::make_shared<Configuration>();

        /* init operator factory */
        std::shared_ptr<AbstractUdfStreamOperator<Function, std::string>> stream_map_1 = std::make_shared<StreamMap<std::string, std::string>>(std::make_shared<StringMapFunction>());
        std::shared_ptr<StreamOperatorFactory<std::string>> operator_factory_1 = SimpleStreamOperatorFactory<std::string>::of(stream_map_1);
        std::shared_ptr<AbstractUdfStreamOperator<Function, std::string>> stream_map_2 = std::make_shared<StreamMap<std::string, std::string>>(std::make_shared<StringMapFunction>());
        std::shared_ptr<StreamOperatorFactory<std::string>> operator_factory_2 = SimpleStreamOperatorFactory<std::string>::of(stream_map_2);
        task_configuration->set_operator_factory<std::string, std::string>(StreamConfig::OPERATOR_FACTORY, operator_factory_1);

        /* init edge */
        std::string node_name("string-map");
        std::shared_ptr<StreamNode<std::string>> src_node = std::make_shared<StreamNode<std::string>>(0, operator_factory_1, node_name);
        std::shared_ptr<StreamNode<std::string>> target_node = std::make_shared<StreamNode<std::string>>(1, operator_factory_2, node_name);
        std::shared_ptr<StreamPartitioner<std::string>> stream_partitioner = std::make_shared<ForwardPartitioner<std::string>>();
        std::shared_ptr<StreamEdge<std::string>> edge = std::make_shared<StreamEdge<std::string>>(src_node, target_node, stream_partitioner);

        task_configuration->set_edge<std::string>(StreamConfig::EDGE_NAME, edge);

        task_configuration->set_value<int>(StreamConfig::NUMBER_OF_INPUTS, std::make_shared<int>(1));

        /* create task_information*/
        std::shared_ptr<TaskInformation> task_information = std::make_shared<TaskInformation>(0, "test-map-task", 1, task_configuration);


        typedef std::vector<std::shared_ptr<ResultPartitionDeploymentDescriptor>> ResultPartitionDeploymentDescriptorList;
        typedef std::vector<std::shared_ptr<InputGateDeploymentDescriptor>> InputGateDeploymentDescriptorList;

        /* create ResultPartition & InputGate descriptors */
        ResultPartitionDeploymentDescriptorList result_partition_descriptors;
        result_partition_descriptors.push_back(std::make_shared<ResultPartitionDeploymentDescriptor>(4));

        InputGateDeploymentDescriptorList input_gate_descriptors;
        input_gate_descriptors.push_back(std::make_shared<InputGateDeploymentDescriptor>(0, new int{0}, 1));

        std::shared_ptr<ResultPartitionManager> result_partition_manager = std::make_shared<ResultPartitionManager>();

        /* InputGate and ResultPartition factory */
        std::shared_ptr<InputGateFactory> input_gate_factory = std::make_shared<InputGateFactory>(result_partition_manager);

        std::shared_ptr<ResultPartitionFactory> result_partition_factory = std::make_shared<ResultPartitionFactory>(result_partition_manager);

        /* Create ShuffleEnvironment */
        std::shared_ptr<ShuffleEnvironment> shuffle_environment = std::make_shared<LocalShuffleEnvironment>(result_partition_manager,
                                                                                                            result_partition_factory,
                                                                                                            input_gate_factory);

        /* Create BufferPool */
        std::shared_ptr<BufferPool> buffer_pool = std::make_shared<BufferPool>(200, 50);
        std::shared_ptr<Task> task = std::make_shared<Task>(job_information, task_information,
                                                            101, 0,
                                                            result_partition_descriptors,
                                                            input_gate_descriptors,
                                                            shuffle_environment,
                                                            "OneInputStreamTask<std::string, std::string>",
                                                            buffer_pool);

        task->start_task_thread();
    }
};