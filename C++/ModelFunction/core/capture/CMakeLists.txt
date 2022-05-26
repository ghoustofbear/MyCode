cmake_minimum_required(VERSION 3.0)

project(libc2cap)

file(GLOB_RECURSE PROJECT_HEADERS *.h)
file(GLOB_RECURSE PROJECT_CPPS    *.cpp)

set(SOURCES ${PROJECT_HEADERS} ${PROJECT_CPPS})

if(WIN32)
    list(APPEND SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/c2cap.rc)
    list(REMOVE_ITEM SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/cap_audio_wave_alsa.cpp)
elseif(APPLE)
    list(REMOVE_ITEM SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/cap_audio_wave_alsa.cpp)
    list(REMOVE_ITEM SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/cap_audio_wave.cpp)
elseif(ANDROID)
    list(REMOVE_ITEM SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/cap_audio_wave_alsa.cpp)
    list(REMOVE_ITEM SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/cap_audio_wave.cpp)
elseif(UNIX)
    list(REMOVE_ITEM SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/cap_audio_wave.cpp)
endif()


source_group_by_dir(SOURCES)

set(EXP_HEADERS c2cap.h)

include_directories(
    .
    ${CORE_SDK_DIR}
    ${CORE_INC_DIR}
    ${CORE_COM_DIR}
    ${CORE_PLY_DIR}
    )

if(MSVC)
    add_definitions(-DCAP_EXPORTS)
    add_definitions(-wd"4819")
elseif(ANDROID)

    link_libraries(
        -lOpenSLES
        )

elseif(UNIX)
    add_definitions(-Wno-deprecated-declarations)
endif()

link_directories(${CORE_LIB_DIR})





if(BUILD_SHARED_LIBS)
    #link_libraries(c2com)
    link_libraries(c2play)

    if(MSVC)
        set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /force:multiple")
    endif()

    add_library(c2cap SHARED ${SOURCES})
    set_target_properties(c2cap PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CORE_CAP_DIR}/${ARCH}
        LIBRARY_OUTPUT_DIRECTORY ${CORE_CAP_DIR}/${ARCH}
        ARCHIVE_OUTPUT_DIRECTORY ${CORE_CAP_DIR}/${ARCH}
        )

    add_custom_command(TARGET c2cap POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/c2cap.h ${SDK_EXP_INC_DIR}
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/c2cap.h ${CORE_INC_DIR}
        COMMENT "This command will be executed after building target c2cap")

    install(TARGETS c2cap
        RUNTIME DESTINATION lib/${OS}/${ARCH}
        LIBRARY DESTINATION lib/${OS}/${ARCH}
        ARCHIVE DESTINATION lib/${OS}/${ARCH}
        )

    if(MSVC)
        add_custom_command(TARGET c2cap POST_BUILD COMMAND auto_copy.bat WORKING_DIRECTORY ${CORE_CAP_DIR})
        install(TARGETS c2cap RUNTIME DESTINATION bin/${OS}/${ARCH}/c2client)
    else()
        add_custom_command(TARGET c2cap POST_BUILD COMMAND bash auto_copy.sh  WORKING_DIRECTORY ${CORE_CAP_DIR})
        install(TARGETS c2cap LIBRARY DESTINATION bin/${OS}/${ARCH}/c2client)
    endif()

endif()

if(BUILD_STATIC_LIBS)
    if(MSVC)
        add_definitions(-DPLAY_STATICLIB -DCM_STATICLIB)
    endif()

    link_libraries(c2play_static c2com_static)

    add_library(c2cap_static STATIC ${SOURCES})
    set_target_properties(c2cap_static PROPERTIES OUTPUT_NAME "c2cap")

    add_custom_command(TARGET c2cap_static POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/c2cap.h ${SDK_EXP_INC_DIR}
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/c2cap.h ${CORE_INC_DIR}
        )

    install(TARGETS c2cap_static
        RUNTIME DESTINATION bin/${OS}/${ARCH}
        LIBRARY DESTINATION lib/${OS}/${ARCH}
        ARCHIVE DESTINATION lib/${OS}/${ARCH}
        )
endif()

install(FILES ${EXP_HEADERS} DESTINATION include/c2cap)
