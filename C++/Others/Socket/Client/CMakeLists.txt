cmake_minimum_required(VERSION 3.10)
project(client)
set(INC_DIR ./include)
include_directories(${INC_DIR})
file(GLOB H_FILE ${INC_DIR}/*.h)
add_executable(${PROJECT_NAME} ${H_FILE}
                            Client.cpp
                                main.cpp)  
#新增如下代码
find_package(Threads REQUIRED) 
target_link_libraries(${PROJECT_NAME} Threads::Threads) 