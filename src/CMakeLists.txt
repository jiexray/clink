# src cmake file
cmake_minimum_required (VERSION 3.13)

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
target_link_libraries(result streamrecord)

target_sources(streamrecord
  PRIVATE
    streamrecord/types/DeserializationDelegate.cpp
    streamrecord/types/IntValue.cpp
    streamrecord/types/DoubleValue.cpp
    streamrecord/types/StringValue.cpp
    streamrecord/typeutils/TypeDeserializer.cpp
    streamrecord/typeutils/TypeSerializerFactory.cpp
)

target_sources(streaming
  PRIVATE
    streaming/task/OperatorChain.cpp
    streaming/task/StreamTask.cpp
    streaming/operators/SimpleStreamOperatorFactory.cpp
    streaming/io/StreamTaskNetworkInput.cpp
    streaming/task/config/StreamConfig.cpp
    core/config/Configuration.cpp
)
target_include_directories(streaming PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/streaming/task
  ${CMAKE_CURRENT_SOURCE_DIR}/streaming/task/config
  ${CMAKE_CURRENT_SOURCE_DIR}/streaming/operators
  ${CMAKE_CURRENT_SOURCE_DIR}/streaming/io
  ${CMAKE_CURRENT_SOURCE_DIR}/streaming/functions
  ${CMAKE_CURRENT_SOURCE_DIR}/streaming/partitioner
  ${CMAKE_CURRENT_SOURCE_DIR}/streaming/graph
  ${CMAKE_CURRENT_SOURCE_DIR}/core/config
  ${CMAKE_CURRENT_SOURCE_DIR}/core/common
  ${CMAKE_CURRENT_SOURCE_DIR}/result
  ${CMAKE_CURRENT_SOURCE_DIR}/result/consumer
  ${CMAKE_CURRENT_SOURCE_DIR}/runtime
)

target_link_libraries(streaming streamrecord result pthread)

# ------------------------
#   build runtime module
# ------------------------
target_sources(runtime 
  PRIVATE
    runtime/Task.cpp
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

target_link_libraries(runtime streaming pthread result streamrecord)



# target_include_directories(result PUBLIC 
#   ${CMAKE_SOURCE_DIR}/include/result 
#   ${CMAKE_SOURCE_DIR}/include/result/consumer
# )
