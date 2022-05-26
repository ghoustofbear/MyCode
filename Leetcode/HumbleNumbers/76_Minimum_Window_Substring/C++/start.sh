#!/bin/bash
cmake .
make
./leetcode
rm cmake_install.cmake
rm CMakeCache.txt
rm -rf CMakeFiles
rm Makefile
rm leetcode