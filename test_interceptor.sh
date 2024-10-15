#!/bin/bash  

# Set LD_PRELOAD to load the interceptor library 
./daemon fileio fileio.txt 1 & 
PID1=$!  

./daemon memmgmt filememmgmt.txt 1 &
PID2=$!  

sleep 2  

LD_PRELOAD=./libc_interceptor.so ./unit_test 
TEST_RESULT=$?  

kill $PID1 || echo "Failed to kill process with PID $PID1"
kill $PID2 || echo "Failed to kill process with PID $PID2"

exit $TEST_RESULT