# test cmake file
cmake_minimum_required (VERSION 3.13)

set(CMAKE_PREFIX_PATH ${CMAKE_SOURCE_DIR}/deps/cxxtest-4.4)
find_package(CxxTest REQUIRED)
# set(CXXTEST_TESTGEN_ARGS "--error-printer --no-eh")

# all include files
set(TEST_HEADER_DIRS ${CMAKE_SOURCE_DIR}/src/buffer
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
  ${CMAKE_SOURCE_DIR}/src/runtime/executiongraph
  ${CMAKE_SOURCE_DIR}/src/runtime/shuffle
  ${CMAKE_SOURCE_DIR}/src/runtime/shuffle/metrics
  ${CMAKE_SOURCE_DIR}/src/runtime/taskexecutor
  ${CMAKE_SOURCE_DIR}/src/metrics
  ${CMAKE_SOURCE_DIR}/src/metrics/groups
  ${CMAKE_SOURCE_DIR}/src/metrics/reporter
  ${CMAKE_SOURCE_DIR}/deps/spdlog/include
  ${CMAKE_SOURCE_DIR}/src/runtime/metrics
  ${CMAKE_SOURCE_DIR}/src/runtime/metrics/groups
  ${CMAKE_SOURCE_DIR}/src/runtime/metrics/scope
  ${CMAKE_SOURCE_DIR}/src/runtime/metrics/util
  )

if(CXXTEST_FOUND)
  include_directories(${CXXTEST_INCLUDE_DIR} ${TEST_HEADER_DIRS} )
  enable_testing()

  # buffer test
  CXXTEST_ADD_TEST(unitest_buffer buffer_test.cpp ${CMAKE_CURRENT_SOURCE_DIR}/buffer/TestBuffer.hpp)
  target_link_libraries(unitest_buffer pthread buffer)

  # buffer pool mananger test
  CXXTEST_ADD_TEST(unitest_buffer_pool_mananger buffer_pool_mananger_test.cpp ${CMAKE_CURRENT_SOURCE_DIR}/buffer/TestBufferPoolManager.hpp)
  target_link_libraries(unitest_buffer_pool_mananger pthread buffer)

  # result test
  CXXTEST_ADD_TEST(unitest_result result_test.cpp ${CMAKE_CURRENT_SOURCE_DIR}/result/TestResult.hpp)
  target_link_libraries(unitest_result result streamrecord runtime)

  # streamrecord test
  CXXTEST_ADD_TEST(unitest_streamrecord streamrecord_test.cpp ${CMAKE_CURRENT_SOURCE_DIR}/streamrecord/TestStreamRecord.hpp)
  target_link_libraries(unitest_streamrecord streamrecord)

  # streaming operator test
  CXXTEST_ADD_TEST(unitest_streaming_operator streaming_operator_test.cpp ${CMAKE_CURRENT_SOURCE_DIR}/streaming/TestOperator.hpp)
  target_link_libraries(unitest_streaming_operator streaming result streamrecord)

  # streaming graph test
  CXXTEST_ADD_TEST(unitest_streaming_graph streaming_graph_test.cpp ${CMAKE_CURRENT_SOURCE_DIR}/streaming/TestGraph.hpp)
  target_link_libraries(unitest_streaming_graph streaming streamrecord result)

  # runtime task test
  CXXTEST_ADD_TEST(unitest_runtime_task runtime_task_test.cpp ${CMAKE_CURRENT_SOURCE_DIR}/runtime/TestTask.hpp)
  target_link_libraries(unitest_runtime_task runtime streaming result streamrecord)

  # runtime task-chain test
  CXXTEST_ADD_TEST(unitest_runtime_task_chain runtime_task_chain_test.cpp ${CMAKE_CURRENT_SOURCE_DIR}/runtime/TestTaskChain.hpp)
  target_link_libraries(unitest_runtime_task_chain runtime streaming result streamrecord)

  # streaming streaming-with-buffer-release test
  CXXTEST_ADD_TEST(unitest_streaming_with_buffer_release streaming_with_buffer_release.cpp ${CMAKE_CURRENT_SOURCE_DIR}/streaming/TestStreamingWithBufferRelease.hpp)
  target_link_libraries(unitest_streaming_with_buffer_release streaming streamrecord result)

  # streaming flat-map test
  CXXTEST_ADD_TEST(unitest_streaming_flat_map_test streaming_flat_map_test.cpp ${CMAKE_CURRENT_SOURCE_DIR}/streaming/TestFlatMap.hpp)
  target_link_libraries(unitest_streaming_flat_map_test streaming streamrecord result)

  # streaming file-read test
  CXXTEST_ADD_TEST(unitest_streaming_file_read_test streaming_file_read_test.cpp ${CMAKE_CURRENT_SOURCE_DIR}/streaming/TestFileRead.hpp)
  target_link_libraries(unitest_streaming_file_read_test streaming)

  # streamrecord tuple test
  CXXTEST_ADD_TEST(unitest_streamrecord_tuple streamrecord_tuple_test.cpp ${CMAKE_CURRENT_SOURCE_DIR}/streamrecord/TestTuple.hpp)
  target_link_libraries(unitest_streamrecord_tuple streamrecord)

  # streaming stream-task-network-input-factory
  CXXTEST_ADD_TEST(unitest_streaming_stream_task_network_input_factory streaming_stream_task_network_input_factory.cpp ${CMAKE_CURRENT_SOURCE_DIR}/streaming/TestStreamTaskNetworkInputFactory.hpp)
  target_link_libraries(unitest_streaming_stream_task_network_input_factory streaming)

  # runtime task-chain-with-tuple test
  CXXTEST_ADD_TEST(unitest_runtime_task_chain_with_tuple runtime_task_chain_with_tuple.cpp ${CMAKE_CURRENT_SOURCE_DIR}/runtime/TestTaskChainWithTuple.hpp)
  target_link_libraries(unitest_runtime_task_chain_with_tuple runtime streaming result streamrecord)

  # runtime wordcount
  CXXTEST_ADD_TEST(unitest_runtime_wordcount runtime_wordcount.cpp ${CMAKE_CURRENT_SOURCE_DIR}/runtime/TestWordCount.hpp)
  target_link_libraries(unitest_runtime_wordcount runtime)

  # core file-output-format
  CXXTEST_ADD_TEST(unitest_core_file_output_format core_file_output_format.cpp ${CMAKE_CURRENT_SOURCE_DIR}/core/TestFileOutputFormat.hpp)
  target_link_libraries(unitest_core_file_output_format core)

  # runtime wordcount-with-file-sink
  CXXTEST_ADD_TEST(unitest_runtime_wordcount_with_file_sink runtime_wordcount_with_file_sink.cpp ${CMAKE_CURRENT_SOURCE_DIR}/runtime/TestWordCountWriteFile.hpp)
  target_link_libraries(unitest_runtime_wordcount_with_file_sink runtime)

  # streaming stream-task-factory
  CXXTEST_ADD_TEST(unitest_streaming_stream_task_factory streaming_stream_task_factory.cpp ${CMAKE_CURRENT_SOURCE_DIR}/streaming/TestStreamTaskFactory.hpp)
  target_link_libraries(unitest_streaming_stream_task_factory streaming runtime)

  # metrics meter
  CXXTEST_ADD_TEST(unitest_metrics_meter metrics_meter.cpp ${CMAKE_CURRENT_SOURCE_DIR}/metrics/TestMeter.hpp)
  target_link_libraries(unitest_metrics_meter runtime)

  # metric scope-format
  CXXTEST_ADD_TEST(unitest_metrics_scope_format metrics_scope_format.cpp ${CMAKE_CURRENT_SOURCE_DIR}/metrics/TestScopeFormat.hpp)
  target_link_libraries(unitest_metrics_scope_format runtime)

  # metrics metric-group
  CXXTEST_ADD_TEST(unitest_metrics_metric_group metrics_metric_group.cpp ${CMAKE_CURRENT_SOURCE_DIR}/metrics/TestMetricGroup.hpp)
  target_link_libraries(unitest_metrics_metric_group runtime)

  # runtime wordcount-with-metrics
  CXXTEST_ADD_TEST(unitest_runtime_wordcount_with_metrics runtime_wordcount_with_metrics.cpp ${CMAKE_CURRENT_SOURCE_DIR}/runtime/TestWordCountWithMetrics.hpp)
  target_link_libraries(unitest_runtime_wordcount_with_metrics runtime)

  # core configuration
  CXXTEST_ADD_TEST(unitest_core_configuration core_configuration.cpp ${CMAKE_CURRENT_SOURCE_DIR}/core/TestConfiguration.hpp)
  target_link_libraries(unitest_core_configuration core)

  # runtime available
  CXXTEST_ADD_TEST(unitest_runtime_available runtime_available.cpp ${CMAKE_CURRENT_SOURCE_DIR}/runtime/TestAvailable.hpp)
  target_link_libraries(unitest_runtime_available runtime)
endif()
