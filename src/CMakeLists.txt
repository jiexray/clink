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
  ${CMAKE_SOURCE_DIR}/src/core/config
  ${CMAKE_SOURCE_DIR}/src/core/common
  ${CMAKE_SOURCE_DIR}/src/core/util
  ${CMAKE_SOURCE_DIR}/src/core/tuple
  ${CMAKE_SOURCE_DIR}/src/runtime/executiongraph
  ${CMAKE_SOURCE_DIR}/src/runtime/shuffle
  ${CMAKE_SOURCE_DIR}/src/runtime/taskexecutor
)

find_package(spdlog REQUIRED)

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
target_link_libraries(core spdlog::spdlog)

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
target_link_libraries(result streamrecord buffer spdlog::spdlog)

# ------------------------
#   build streamrecord module
# ------------------------
target_sources(streamrecord
  PRIVATE
    streamrecord/types/TupleDeserializationDelegate.cpp
    streamrecord/types/IntValue.cpp
    streamrecord/types/DoubleValue.cpp
    streamrecord/types/StringValue.cpp
    streamrecord/typeutils/deserialize/TypeDeserializerImpl.cpp
    streamrecord/typeutils/serialize/TypeSerializerFactory.cpp
    streamrecord/typeutils/serialize/IntSerializer.cpp
    streamrecord/typeutils/serialize/StringSerializer.cpp
    streamrecord/typeutils/serialize/DoubleSerializer.cpp
    streamrecord/typeutils/deserialize/TupleDeserializer.cpp
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

target_link_libraries(streaming streamrecord result pthread core buffer spdlog::spdlog)

# ------------------------
#   build runtime module
# ------------------------

target_sources(runtime 
  PRIVATE
    runtime/Task.cpp
    runtime/taskexecutor/TaskSlotTable.cpp
    runtime/taskexecutor/TaskExecutor.cpp
    runtime/shuffle/ShuffleEnvironment.cpp
)
target_include_directories(runtime PUBLIC
  ${ALL_INCLUDE_DIRS}
)

target_link_libraries(runtime streaming pthread result streamrecord spdlog::spdlog buffer)



# target_include_directories(result PUBLIC 
#   ${CMAKE_SOURCE_DIR}/include/result 
#   ${CMAKE_SOURCE_DIR}/include/result/consumer
# )
