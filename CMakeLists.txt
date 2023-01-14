cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)

project(srpc)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SRPC_SRCS
  src/protocol/string.cc
)
add_library(srpc SHARED)
target_include_directories(srpc PUBLIC src)
target_sources(srpc PRIVATE ${SRPC_SRCS})

if(BUILD_TESTING)
  enable_testing()
  add_subdirectory(test)
endif()
