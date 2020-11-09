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
  target_link_libraries(unitest_result result runtime)

  # core file-output-format
  CXXTEST_ADD_TEST(unitest_core_file_output_format core_file_output_format.cpp ${CMAKE_CURRENT_SOURCE_DIR}/core/TestFileOutputFormat.hpp)
  target_link_libraries(unitest_core_file_output_format core)

  # runtime wordcount-with-file-sink
  CXXTEST_ADD_TEST(unitest_runtime_wordcount_with_file_sink runtime_wordcount_with_file_sink.cpp ${CMAKE_CURRENT_SOURCE_DIR}/runtime/TestWordCountWriteFile.hpp)
  target_link_libraries(unitest_runtime_wordcount_with_file_sink runtime)

  # metrics meter
  CXXTEST_ADD_TEST(unitest_metrics_meter metrics_meter.cpp ${CMAKE_CURRENT_SOURCE_DIR}/metrics/TestMeter.hpp)
  target_link_libraries(unitest_metrics_meter runtime)

  # metric scope-format
  CXXTEST_ADD_TEST(unitest_metrics_scope_format metrics_scope_format.cpp ${CMAKE_CURRENT_SOURCE_DIR}/metrics/TestScopeFormat.hpp)
  target_link_libraries(unitest_metrics_scope_format runtime)

  # metrics metric-group
  CXXTEST_ADD_TEST(unitest_metrics_metric_group metrics_metric_group.cpp ${CMAKE_CURRENT_SOURCE_DIR}/metrics/TestMetricGroup.hpp)
  target_link_libraries(unitest_metrics_metric_group runtime)

  # core configuration
  CXXTEST_ADD_TEST(unitest_core_configuration core_configuration.cpp ${CMAKE_CURRENT_SOURCE_DIR}/core/TestConfiguration.hpp)
  target_link_libraries(unitest_core_configuration core)

  # runtime available
  CXXTEST_ADD_TEST(unitest_runtime_available runtime_available.cpp ${CMAKE_CURRENT_SOURCE_DIR}/runtime/TestAvailable.hpp)
  target_link_libraries(unitest_runtime_available runtime)

  # runtime file read throughput
  CXXTEST_ADD_TEST(unitest_runtime_file_read_throughput runtime_file_read_throughput.cpp ${CMAKE_CURRENT_SOURCE_DIR}/runtime/TestFileReadThroughput.hpp)
  target_link_libraries(unitest_runtime_file_read_throughput runtime)

  # runtime wordcount file read with new filereader
  CXXTEST_ADD_TEST(unitest_runtime_wordcount_with_new_filereader runtime_wordcount_with_new_filereader.cpp ${CMAKE_CURRENT_SOURCE_DIR}/runtime/TestWordcountWithNewFileReader.hpp)
  target_link_libraries(unitest_runtime_wordcount_with_new_filereader runtime)

  # runtime file read string throughput
  CXXTEST_ADD_TEST(unitest_runtime_file_read_string_throughput runtime_file_read_string_throughput.cpp ${CMAKE_CURRENT_SOURCE_DIR}/runtime/TestFileReadStringThroughput.hpp)
  target_link_libraries(unitest_runtime_file_read_string_throughput runtime)

  # runtime file read throughput mid operator throughput
  CXXTEST_ADD_TEST(unitest_runtime_file_read_mid_operator_throughput runtime_file_read_mid_operator.cpp ${CMAKE_CURRENT_SOURCE_DIR}/runtime/TestFileReadMidOperatorThroughput.hpp)
  target_link_libraries(unitest_runtime_file_read_mid_operator_throughput runtime)

  # streamrecord streamrecordV2
  CXXTEST_ADD_TEST(unitest_streamrecord_V2 streamRecord_V2.cpp ${CMAKE_CURRENT_SOURCE_DIR}/streamrecord/TestStreamRecordV2.hpp)
  target_link_libraries(unitest_streamrecord_V2 streamrecord)
endif()
