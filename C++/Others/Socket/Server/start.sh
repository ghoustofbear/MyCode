#!/bin/bash
cmake .
make
chmod u+x Server
./Server
rm cmake_install.cmake
rm CMakeCache.txt
rm -rf CMakeFiles
rm Makefile