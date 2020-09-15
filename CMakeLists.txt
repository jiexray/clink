# root cmake file
cmake_minimum_required (VERSION 3.13)

project (clink)

add_compile_definitions(SPDLOG_ACTIVE_LEVEL=0)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(buffer "")
add_library(result "")
add_library(streamrecord "")
add_library(streaming "")
add_library(runtime "")
add_library(core "")

# install (TARGETS result
#   PUBLIC_HEADER
#     DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
# )

add_subdirectory (test)
add_subdirectory (src)

# add_test (NAME BufferTest COMMAND BufferTest)
