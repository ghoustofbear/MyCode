#!/bin/bash
cmake .
make
./QuickSort
rm cmake_install.cmake
rm CMakeCache.txt
rm -rf CMakeFiles
rm Makefile
rm QuickSort