#!/bin/bash
cmake .
make
./test
rm cmake_install.cmake
rm CMakeCache.txt
rm -rf CMakeFiles
rm Makefile
rm test