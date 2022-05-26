#!/bin/bash
cmake .
make
./client
rm cmake_install.cmake
rm CMakeCache.txt
rm -rf CMakeFiles
rm Makefile