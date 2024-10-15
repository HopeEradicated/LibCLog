cmake_minimum_required(VERSION 3.10)

project(LibCLog)

add_library(libc_interceptor SHARED libc_interceptor.cpp)

target_link_libraries(libc_interceptor dl)

add_executable(daemon daemon.cpp)
add_executable(unit_test unit_test.cpp)

enable_testing()
add_custom_target(test_interceptor
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/test_interceptor.sh 
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

message(STATUS "Build directory: ${CMAKE_BINARY_DIR}")