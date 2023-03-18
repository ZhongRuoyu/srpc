cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)

function(srpc_add_example SRC)
  get_filename_component(exe_name ${SRC} NAME_WE)
  add_executable(${exe_name} ${SRC})
  target_link_libraries(${exe_name} PRIVATE srpc)
endfunction()

set(SRPC_EXAMPLES
  tcp_client.cc
  tcp_server.cc
  udp_client.cc
  udp_server.cc
)
foreach(example ${SRPC_EXAMPLES})
  srpc_add_example(${example})
endforeach()
