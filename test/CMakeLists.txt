# test cmake file
cmake_minimum_required (VERSION 3.13)

set(CMAKE_PREFIX_PATH ${CMAKE_SOURCE_DIR}/deps/cxxtest-4.4)
find_package(CxxTest REQUIRED)

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
  )

if(CXXTEST_FOUND)
  include_directories(${CXXTEST_INCLUDE_DIR} ${TEST_HEADER_DIRS} )
  enable_testing()

  # buffer test
  CXXTEST_ADD_TEST(unitest_buffer buffer_test.cpp ${CMAKE_CURRENT_SOURCE_DIR}/buffer/TestBuffer.hpp)
  target_link_libraries(unitest_buffer pthread)

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

endif()
