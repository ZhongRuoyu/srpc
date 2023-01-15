cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)

find_package(GTest REQUIRED)
add_executable(srpc_tests)
target_sources(srpc_tests PRIVATE
  srpc/protocol/floats.cc
  srpc/protocol/integers.cc
  srpc/protocol/strings.cc
  srpc/utils/memory.cc
  srpc/utils/result.cc
)
target_link_libraries(srpc_tests PRIVATE
  srpc
  GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(srpc_tests)
