#!/bin/bash
cmake .
make
./SelectSort
rm cmake_install.cmake
rm CMakeCache.txt
rm -rf CMakeFiles
rm Makefile
rm SelectSort