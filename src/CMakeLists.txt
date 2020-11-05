# src cmake file
cmake_minimum_required (VERSION 3.13)

find_package(spdlog REQUIRED)
# find_package(tbb REQUIRED)
find_package(Boost COMPONENTS system thread REQUIRED)

# all include files
set(ALL_INCLUDE_DIRS 
  ${Boost_INCLUDE_DIR}
  ${CMAKE_SOURCE_DIR}/src/buffer
  ${CMAKE_SOURCE_DIR}/src/result 
  ${CMAKE_SOURCE_DIR}/src/result/consumer
  ${CMAKE_SOURCE_DIR}/src/deployment
  ${CMAKE_SOURCE_DIR}/src/streaming/partitioner
  ${CMAKE_SOURCE_DIR}/src/streamrecord
  ${CMAKE_SOURCE_DIR}/src/streamrecord/types
  ${CMAKE_SOURCE_DIR}/src/streamrecord/typeutils
  ${CMAKE_SOURCE_DIR}/src/streamrecord/typeutils/serialize
  ${CMAKE_SOURCE_DIR}/src/streamrecord/typeutils/deserialize
  ${CMAKE_SOURCE_DIR}/src/streaming/functions
  ${CMAKE_SOURCE_DIR}/src/streaming/io
  ${CMAKE_SOURCE_DIR}/src/streaming/operators
  ${CMAKE_SOURCE_DIR}/src/streaming/partitioner
  ${CMAKE_SOURCE_DIR}/src/streaming/task
  ${CMAKE_SOURCE_DIR}/src/streaming/task/config
  ${CMAKE_SOURCE_DIR}/src/streaming/task/mailbox
  ${CMAKE_SOURCE_DIR}/src/streaming/graph
  ${CMAKE_SOURCE_DIR}/src/runtime
  ${CMAKE_SOURCE_DIR}/src/runtime/io
  ${CMAKE_SOURCE_DIR}/src/core/config
  ${CMAKE_SOURCE_DIR}/src/core/common
  ${CMAKE_SOURCE_DIR}/src/core/common/io
  ${CMAKE_SOURCE_DIR}/src/core/util
  ${CMAKE_SOURCE_DIR}/src/core/tuple
  ${CMAKE_SOURCE_DIR}/src/runtime/executiongraph
  ${CMAKE_SOURCE_DIR}/src/runtime/shuffle
  ${CMAKE_SOURCE_DIR}/src/runtime/shuffle/metrics
  ${CMAKE_SOURCE_DIR}/src/runtime/taskexecutor
  ${CMAKE_SOURCE_DIR}/src/metrics
  ${CMAKE_SOURCE_DIR}/src/metrics/reporter
  ${CMAKE_SOURCE_DIR}/src/metrics/groups
  ${CMAKE_SOURCE_DIR}/src/runtime/metrics
  ${CMAKE_SOURCE_DIR}/src/runtime/metrics/groups
  ${CMAKE_SOURCE_DIR}/src/runtime/metrics/scope
  ${CMAKE_SOURCE_DIR}/src/runtime/metrics/util
)


# ------------------------
#   build buffer module
# ------------------------
target_sources(buffer
  PRIVATE
    buffer/BufferPool.cpp
    buffer/BufferBuilder.cpp
    buffer/BufferConsumer.cpp
    buffer/ReadOnlySlidedBuffer.cpp
    buffer/Buffer.cpp
    buffer/BufferPoolManager.cpp
)
target_include_directories(buffer PUBLIC
  ${ALL_INCLUDE_DIRS}
)
target_link_libraries(buffer core)

# ------------------------
#   build core module
# ------------------------
target_sources(core
  PRIVATE
    core/config/Constant.cpp
)
target_include_directories(core PUBLIC
  ${ALL_INCLUDE_DIRS}
)
target_link_libraries(core spdlog::spdlog ${Boost_LIBRARIES})

# ------------------------
#   build result module
# ------------------------
target_sources(result
  PRIVATE
    result/ResultPartition.cpp
    result/ResultPartitionManager.cpp
    result/ResultSubpartition.cpp
    result/ResultSubpartitionView.cpp
    result/consumer/InputChannel.cpp
    result/consumer/InputGate.cpp
    result/consumer/InputGateFactory.cpp
)
target_link_libraries(result streamrecord buffer spdlog::spdlog tbb)

# ------------------------
#   build streamrecord module
# ------------------------
target_sources(streamrecord
  PRIVATE
    # streamrecord/types/TupleDeserializationDelegate.cpp
    streamrecord/types/IntValue.cpp
    streamrecord/types/DoubleValue.cpp
    streamrecord/types/StringValue.cpp
    streamrecord/typeutils/deserialize/TypeDeserializerImpl.cpp
    streamrecord/typeutils/serialize/TypeSerializerFactory.cpp
    streamrecord/typeutils/serialize/IntSerializer.cpp
    streamrecord/typeutils/serialize/StringSerializer.cpp
    streamrecord/typeutils/serialize/DoubleSerializer.cpp
    streamrecord/typeutils/deserialize/TupleDeserializer.cpp
    streamrecord/typeutils/deserialize/TupleDeserializerV2.cpp
)
target_include_directories(streamrecord PUBLIC
  ${ALL_INCLUDE_DIRS}
)
target_link_libraries(streamrecord core buffer spdlog::spdlog)

# ------------------------
#   build streaming module
# ------------------------

target_sources(streaming
  PRIVATE
    streaming/task/config/StreamConfig.cpp
    streaming/task/mailbox/TaskMailbox.cpp
    streaming/task/mailbox/MailboxProcessor.cpp
)
target_include_directories(streaming PUBLIC
  ${ALL_INCLUDE_DIRS}
)

target_link_libraries(streaming streamrecord result pthread core buffer spdlog::spdlog runtime)

# ------------------------
#   build runtime module
# ------------------------

target_sources(runtime 
  PRIVATE
    runtime/Task.cpp
    runtime/taskexecutor/TaskSlotTable.cpp
    runtime/taskexecutor/TaskExecutor.cpp
    runtime/taskexecutor/TaskExecutorRunner.cpp
    runtime/shuffle/ShuffleEnvironment.cpp
    runtime/shuffle/metrics/ShuffleMetricFactory.cpp
    runtime/shuffle/metrics/InputChannelMetrics.cpp

    runtime/metrics/groups/TaskManagerMetricGroup.cpp
    runtime/metrics/groups/TaskManagerJobMetricGroup.cpp
    runtime/metrics/groups/TaskMetricGroup.cpp
    runtime/metrics/groups/TaskIOMetricGroup.cpp
    runtime/metrics/groups/OperatorMetricGroup.cpp
    runtime/metrics/groups/OperatorIOMetricGroup.cpp


    runtime/metrics/MetricRegistry.cpp
    runtime/metrics/MetricNames.cpp
    runtime/metrics/ReporterSetup.cpp
    runtime/metrics/MetricRegistryConfiguration.cpp

    runtime/metrics/scope/ScopeFormat.cpp
    runtime/metrics/scope/ScopeFormats.cpp
    runtime/metrics/scope/TaskManagerJobScopeFormat.cpp
    runtime/metrics/scope/TaskScopeFormat.cpp
    runtime/metrics/scope/OperatorScopeFormat.cpp

    runtime/metrics/util/MetricUtils.cpp

    runtime/io/AvailabilityProvider.cpp
)
target_include_directories(runtime PUBLIC
  ${ALL_INCLUDE_DIRS}
)

target_link_libraries(runtime metrics streaming pthread result streamrecord spdlog::spdlog buffer ${Boost_LIBRARIES})

# ------------------------
#   build metrics module
# ------------------------
target_sources(metrics 
  PRIVATE
    metrics/MeterView.cpp
    metrics/View.cpp
    metrics/reporter/LoggerReporter.cpp
    metrics/MetricConfig.cpp
)
target_include_directories(metrics PUBLIC
  ${ALL_INCLUDE_DIRS}
)

target_link_libraries(metrics core)