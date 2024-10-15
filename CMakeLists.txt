cmake_minimum_required(VERSION 3.10)

project(LibCLog)

add_library(libc_interceptor SHARED libc_interceptor.cpp)

target_link_libraries(libc_interceptor dl)

add_executable(daemon daemon.cpp)
add_executable(unit_test unit_test.cpp)

add_test(NAME daemon_fileio COMMAND daemon fileio fileio.txt 1)
add_test(NAME daemon_memmgmt COMMAND daemon memmgmt memmgmt.txt 1)
add_test(NAME unit_test_intercept COMMAND export LD_PRELOAD=./libc_interceptor.so ./unit_test)

add_custom_target(run_daemons
  COMMAND daemon fileio fileio.txt 1 &
  COMMAND daemon memmgmt memmgmt.txt 1 &
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)