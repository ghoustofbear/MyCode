cmake_minimum_required(VERSION 3.10)
project(INIFILE)
set(INC_DIR 
    ./include)
include_directories(${INC_DIR})
message(STATUS,"INC_DIR is ${INC_DIR}")
file(GLOB ALL_CPPS ${INC_DIR}/src/*.cpp)
file(GLOB ALL_HPPS ${INC_DIR}/include/*.h)
add_executable(${PROJECT_NAME}
                ALL_CPPS
                ALL_HPPS
                main.cpp)