#!/bin/bash
cmake .
make
./client
rm -f cmake_install.cmake
rm -f CMakeCache.txt
rm -rf CMakeFiles
rm Makefile
rm client