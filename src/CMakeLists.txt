# src cmake file
cmake_minimum_required (VERSION 3.13)

# all include files
set(ALL_INCLUDE_DIRS 
  ${CMAKE_SOURCE_DIR}/src/buffer
  ${CMAKE_SOURCE_DIR}/src/result 
  ${CMAKE_SOURCE_DIR}/src/result/consumer
  ${CMAKE_SOURCE_DIR}/src/deployment
  ${CMAKE_SOURCE_DIR}/src/streaming/partitioner
  ${CMAKE_SOURCE_DIR}/src/streamrecord
  ${CMAKE_SOURCE_DIR}/src/streamrecord/types
  ${CMAKE_SOURCE_DIR}/src/streamrecord/typeutils
  ${CMAKE_SOURCE_DIR}/src/streaming/functions
  ${CMAKE_SOURCE_DIR}/src/streaming/io
  ${CMAKE_SOURCE_DIR}/src/streaming/operators
  ${CMAKE_SOURCE_DIR}/src/streaming/partitioner
  ${CMAKE_SOURCE_DIR}/src/streaming/task
  ${CMAKE_SOURCE_DIR}/src/streaming/task/config
  ${CMAKE_SOURCE_DIR}/src/streaming/task/mailbox
  ${CMAKE_SOURCE_DIR}/src/streaming/graph
  ${CMAKE_SOURCE_DIR}/src/runtime
  ${CMAKE_SOURCE_DIR}/src/core/config
  ${CMAKE_SOURCE_DIR}/src/core/common
  ${CMAKE_SOURCE_DIR}/src/core/util
  ${CMAKE_SOURCE_DIR}/src/runtime/executiongraph
  ${CMAKE_SOURCE_DIR}/src/runtime/shuffle
  ${CMAKE_SOURCE_DIR}/src/runtime/taskexecutor
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
    core/config/Configuration.cpp
    core/config/Constant.cpp
)
target_include_directories(core PUBLIC
  ${ALL_INCLUDE_DIRS}
)

# ------------------------
#   build result module
# ------------------------
target_sources(result
  PRIVATE
    result/ResultPartition.cpp
    result/ResultPartitionManager.cpp
    result/ResultSubpartition.cpp
    result/ResultSubpartitionView.cpp
    result/ResultWriter.cpp
    result/consumer/InputChannel.cpp
    result/consumer/InputGate.cpp
    result/consumer/InputGateFactory.cpp
)
target_link_libraries(result streamrecord buffer)

# ------------------------
#   build streamrecord module
# ------------------------
target_sources(streamrecord
  PRIVATE
    streamrecord/types/DeserializationDelegate.cpp
    streamrecord/types/IntValue.cpp
    streamrecord/types/DoubleValue.cpp
    streamrecord/types/StringValue.cpp
    streamrecord/typeutils/TypeDeserializer.cpp
    streamrecord/typeutils/TypeSerializerFactory.cpp
)
target_include_directories(streamrecord PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/core/config
)
target_link_libraries(streamrecord core buffer)

# ------------------------
#   build streaming module
# ------------------------

target_sources(streaming
  PRIVATE
    streaming/task/OperatorChain.cpp
    streaming/task/StreamTask.cpp
    streaming/operators/SimpleStreamOperatorFactory.cpp
    streaming/io/StreamTaskNetworkInput.cpp
    streaming/task/config/StreamConfig.cpp
    streaming/task/mailbox/TaskMailbox.cpp
    streaming/task/mailbox/MailboxProcessor.cpp
)
target_include_directories(streaming PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/streaming/task
  ${CMAKE_CURRENT_SOURCE_DIR}/streaming/task/config
  ${CMAKE_CURRENT_SOURCE_DIR}/streaming/operators
  ${CMAKE_CURRENT_SOURCE_DIR}/streaming/io
  ${CMAKE_CURRENT_SOURCE_DIR}/streaming/functions
  ${CMAKE_CURRENT_SOURCE_DIR}/streaming/partitioner
  ${CMAKE_CURRENT_SOURCE_DIR}/streaming/graph
  ${CMAKE_CURRENT_SOURCE_DIR}/streaming/task/mailbox
  ${CMAKE_CURRENT_SOURCE_DIR}/core/config
  ${CMAKE_CURRENT_SOURCE_DIR}/core/common
  ${CMAKE_CURRENT_SOURCE_DIR}/core/util
  ${CMAKE_CURRENT_SOURCE_DIR}/result
  ${CMAKE_CURRENT_SOURCE_DIR}/result/consumer
  ${CMAKE_CURRENT_SOURCE_DIR}/runtime
)

target_link_libraries(streaming streamrecord result pthread core buffer)

# ------------------------
#   build runtime module
# ------------------------
# set(SPDLOG_DIR /home/tian/software/vscode/projects/clink/deps/spdlog/build)

find_package(spdlog REQUIRED)

target_sources(runtime 
  PRIVATE
    runtime/Task.cpp
    runtime/taskexecutor/TaskSlotTable.cpp
    runtime/taskexecutor/TaskExecutor.cpp
    runtime/shuffle/ShuffleEnvironment.cpp
)
target_include_directories(runtime PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/core/common
  ${CMAKE_CURRENT_SOURCE_DIR}/core/config
  ${CMAKE_CURRENT_SOURCE_DIR}/result
  ${CMAKE_CURRENT_SOURCE_DIR}/result/consumer
  ${CMAKE_CURRENT_SOURCE_DIR}/runtime/executiongraph
  ${CMAKE_CURRENT_SOURCE_DIR}/runtime/shuffle
  ${CMAKE_CURRENT_SOURCE_DIR}/deployment
)

target_link_libraries(runtime streaming pthread result streamrecord spdlog::spdlog buffer)



# target_include_directories(result PUBLIC 
#   ${CMAKE_SOURCE_DIR}/include/result 
#   ${CMAKE_SOURCE_DIR}/include/result/consumer
# )
