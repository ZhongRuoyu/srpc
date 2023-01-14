cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)

find_package(GTest REQUIRED)
add_executable(srpc_tests)
target_include_directories(srpc_tests PRIVATE ../src)
target_sources(srpc_tests PRIVATE
  protocol/floats.cc
  protocol/integers.cc
  protocol/string.cc
  utils/memory.cc
)
target_link_libraries(srpc_tests PRIVATE
  srpc
  GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(srpc_tests)
