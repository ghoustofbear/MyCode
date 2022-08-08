#!/bin/bash
cmake .
make
./03longest-substring-without-repeating-characters
rm cmake_install.cmake
rm CMakeCache.txt
rm -rf CMakeFiles
rm Makefile
rm 03longest-substring-without-repeating-characters