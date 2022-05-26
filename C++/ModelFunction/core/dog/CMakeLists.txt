cmake_minimum_required(VERSION 3.0)

project(watchdog)

file(GLOB_RECURSE PROJECT_HEADERS *.h)
file(GLOB_RECURSE PROJECT_CPPS    *.cpp)

set(SOURCES ${PROJECT_HEADERS} ${PROJECT_CPPS})

source_group_by_dir(SOURCES)

if(MSVC)
    add_definitions(-wd"4819")
else()
    add_definitions(-Wno-deprecated-declarations)
endif()


include_directories(
    .
    ${CORE_COM_DIR}
    ${CORE_INC_DIR}
    )

link_directories(${CORE_LIB_DIR})

link_libraries(
    c2com
    debug jsoncpp${CMAKE_DEBUG_POSTFIX}     optimized jsoncpp
    -lpthread
    )
    


add_executable(watchdog ${SOURCES})

set_target_properties(watchdog PROPERTIES
    OUTPUT_NAME                      "watchdog"
    DEBUG_OUTPUT_NAME                "watchdog${CMAKE_DEBUG_POSTFIX}"
    RUNTIME_OUTPUT_DIRECTORY         ${MSV_EXP_DLL_DIR}
    RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${MSV_EXP_DLL_DIR}
    VS_DEBUGGER_WORKING_DIRECTORY    ${MSV_EXP_DLL_DIR}
    )

install(TARGETS watchdog DESTINATION bin/${OS}/${ARCH}/c2server)
install(FILES ${MSV_RC_DIR}/watchdog.json DESTINATION bin/${OS}/${ARCH}/c2server)
install(FILES ${MSV_RC_DIR}/watchdog_multServer.json DESTINATION bin/${OS}/${ARCH}/c2server)
