cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)

project(srpc)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SRPC_HDRS
  include/srpc/network/client.h
  include/srpc/network/datagram_client.h
  include/srpc/network/datagram_server.h
  include/srpc/network/datagram_socket.h
  include/srpc/network/message.h
  include/srpc/network/server.h
  include/srpc/network/server_datagram_socket.h
  include/srpc/network/server_socket.h
  include/srpc/network/socket.h
  include/srpc/network/tcp_ip.h
  include/srpc/types/floats.h
  include/srpc/types/integers.h
  include/srpc/types/serialization.h
  include/srpc/types/strings.h
  include/srpc/types/vectors.h
  include/srpc/utils/result.h
)
set(SRPC_SRCS
  src/srpc/network/client.cc
  src/srpc/network/datagram_client.cc
  src/srpc/network/datagram_server.cc
  src/srpc/network/datagram_socket.cc
  src/srpc/network/message.cc
  src/srpc/network/server.cc
  src/srpc/network/server_datagram_socket.cc
  src/srpc/network/server_socket.cc
  src/srpc/network/socket.cc
  src/srpc/types/strings.cc
  src/srpc/types/vectors.cc
)

option(BUILD_SHARED_LIBS "Build srpc as a shared library" ON)
add_library(srpc)
target_include_directories(srpc PUBLIC include)
target_include_directories(srpc PRIVATE src)
target_sources(srpc PRIVATE ${SRPC_SRCS})

find_package(Threads REQUIRED)
target_link_libraries(srpc PUBLIC Threads::Threads)

include(GNUInstallDirs)
install(TARGETS srpc DESTINATION ${CMAKE_INSTALL_LIBDIR})
foreach(header ${SRPC_HDRS})
  string(REGEX REPLACE "^include/(.*)/.*$" "\\1" header_dir ${header})
  install(FILES ${header}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${header_dir})
endforeach()

option(BUILD_TESTING "Build tests" OFF)
if(BUILD_TESTING)
  enable_testing()
  add_subdirectory(test)
endif()

option(BUILD_EXAMPLES "Build examples" OFF)
if(BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()
