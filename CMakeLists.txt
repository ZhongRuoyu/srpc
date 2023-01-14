cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)

project(srpc)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(srpc OBJECT
  src/protocol/floats.cc
  src/protocol/integers.cc
  src/protocol/string.cc
  src/utils/memory.cc
)
target_include_directories(srpc PRIVATE src)

if(BUILD_TESTING)
  enable_testing()
  add_subdirectory(test)
endif()
