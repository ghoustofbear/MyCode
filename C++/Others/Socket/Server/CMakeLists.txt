cmake_minimum_required(VERSION 3.10)
project(Server)
set(SERVER_INC ./include)
message(STATUS,"SERVER_INC is ${SERVER_INC}")
include_directories(${SERVER_INC})
file(GLOB H_FILE ${SERVER_INC}/*.h)
message(STATUS,"H_FILE is ${H_FILE}")
set(SOURCE 
                main.cpp
                
                Server.cpp
                ${H_FILE})
message(STATUS,"SOURCE is ${SOURCE}")
add_executable(${PROJECT_NAME} ${SOURCE})   

find_package(Threads REQUIRED) 
target_link_libraries(${PROJECT_NAME} Threads::Threads)

