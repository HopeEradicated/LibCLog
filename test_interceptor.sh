#!/bin/bash

# Compile the test program
g++ -o unit_test unite_test.cpp
g++ -o daemon daemon.cpp
g++ -shared -fPIC -o libc_interceptor.so libc_interceptor.cpp -ldl

# Set LD_PRELOAD to load the interceptor library
./daemon fileio fileio.txt 1 
./daemon memmgmt filememmgmt.txt 1
LD_PRELOAD=./libc_interceptor.so ./unit_test