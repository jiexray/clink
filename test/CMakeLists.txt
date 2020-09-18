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
  ${CMAKE_SOURCE_DIR}/src/runtime/executiongraph
  ${CMAKE_SOURCE_DIR}/src/runtime/shuffle
  ${CMAKE_SOURCE_DIR}/src/runtime/taskexecutor
  ${CMAKE_SOURCE_DIR}/deps/spdlog/include
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
  target_link_libraries(unitest_result result streamrecord)

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
endif()
