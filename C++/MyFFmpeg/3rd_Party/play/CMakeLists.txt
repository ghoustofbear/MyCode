cmake_minimum_required(VERSION 3.0)

project(libc2play)

include_directories(${CORE_INC_DIR} ${CORE_COM_DIR})

file(GLOB_RECURSE PROJECT_HEADERS *.h)
file(GLOB_RECURSE PROJECT_CPPS    *.cpp)

set(SOURCES ${PROJECT_HEADERS} ${PROJECT_CPPS})

if(MSVC)
    list(APPEND SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/c2play.rc)
    list(REMOVE_ITEM SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/render_gl.cpp)
    list(REMOVE_ITEM SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/render_gles2.cpp)
    list(REMOVE_ITEM SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/track_sles.cpp)
elseif(ANDROID)
    list(REMOVE_ITEM SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/render_d3d.cpp)
    list(REMOVE_ITEM SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/track_wave.cpp)
    list(REMOVE_ITEM SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/render_sdl.cpp)
    list(REMOVE_ITEM SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/track_sdl.cpp)
elseif(UNIX)
    list(REMOVE_ITEM SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/render_d3d.cpp)
    list(REMOVE_ITEM SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/track_wave.cpp)
    list(REMOVE_ITEM SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/render_gl.cpp)
    list(REMOVE_ITEM SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/render_gles2.cpp)
    list(REMOVE_ITEM SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/track_sles.cpp)
endif()

source_group_by_dir(SOURCES)

set(EXP_HEADERS c2play.h)

if(MSVC)
    add_definitions(-DPLAY_EXPORTS)
    add_definitions(-wd"4819")
else()
    add_definitions(-Wno-deprecated-declarations)
endif()

link_directories(${CORE_LIB_DIR})

if(MSVC)
    link_libraries(
        debug libavcodec${CMAKE_DEBUG_POSTFIX}    optimized libavcodec
        debug libavdevice${CMAKE_DEBUG_POSTFIX}   optimized libavdevice
        debug libavfilter${CMAKE_DEBUG_POSTFIX}   optimized libavfilter
        debug libavformat${CMAKE_DEBUG_POSTFIX}   optimized libavformat
        debug libavutil${CMAKE_DEBUG_POSTFIX}     optimized libavutil
        debug libpostproc${CMAKE_DEBUG_POSTFIX}   optimized libpostproc
        debug libswresample${CMAKE_DEBUG_POSTFIX} optimized libswresample
        debug libswscale${CMAKE_DEBUG_POSTFIX}    optimized libswscale
        debug libx264${CMAKE_DEBUG_POSTFIX}       optimized libx264
        debug libx265${CMAKE_DEBUG_POSTFIX}       optimized libx265
        )
elseif(ANDROID)

    link_libraries(
        debug x264${CMAKE_DEBUG_POSTFIX} optimized x264
        debug x265${CMAKE_DEBUG_POSTFIX} optimized x265
        avformat
        avdevice
        avfilter
        avcodec
        postproc
        swresample
        swscale
        avutil
        -lEGL
        -lGLESv1_CM
        -lGLESv2
        -lOpenSLES
        -landroid
        -lm
        -llog
        -ldl
        -lz
    )
elseif(UNIX)

    #include_directories(/home/songhongliang/workspace/ndk/android-ndk-r10e/platforms/android-21/arch-arm/usr/include/)

    link_libraries(
        avformat
        avdevice
        avfilter
        avcodec
        postproc
        swresample
        swscale
        avutil
        debug x264${CMAKE_DEBUG_POSTFIX} optimized x264
        debug x265${CMAKE_DEBUG_POSTFIX} optimized x265
        -lasound
        -lxcb
        -lxcb-shm
        -lxcb-xfixes
        -lxcb-shape
        -ldl
        -lz
        )
endif()

link_libraries(
    debug SDL2${CMAKE_DEBUG_POSTFIX}     optimized SDL2
    debug SDL2main${CMAKE_DEBUG_POSTFIX} optimized SDL2main
    )

if(MSVC)
    link_libraries(d3d9 winmm Version imm32 secur32.lib strmiids.lib dxguid.lib shlwapi.lib vfw32.lib)
endif()

add_definitions(-DLOG4X_STATICLIB)

if(BUILD_SHARED_LIBS)
    if(MSVC)
        #set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /NODEFAULTLIB:libcmt.lib")
        set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS}  /ignore:4049 /ignore:4217")
    else()
        link_libraries(-Wl,-Bsymbolic)
    endif()

    if(ANDROID)
        link_libraries(c2com_static log4x)
    else()
        link_libraries(c2com)
    endif()

    add_library(c2play SHARED ${SOURCES})
    set_target_properties(c2play PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CORE_PLY_DIR}/${ARCH}
        LIBRARY_OUTPUT_DIRECTORY ${CORE_PLY_DIR}/${ARCH}
        ARCHIVE_OUTPUT_DIRECTORY ${CORE_PLY_DIR}/${ARCH}
        )

    install(TARGETS c2play
        RUNTIME DESTINATION lib/${OS}/${ARCH}
        LIBRARY DESTINATION lib/${OS}/${ARCH}
        ARCHIVE DESTINATION lib/${OS}/${ARCH}
        )

    if(MSVC)
        add_custom_command(TARGET c2play
            POST_BUILD
            COMMAND auto_copy.bat
            WORKING_DIRECTORY ${CORE_PLY_DIR}
            COMMENT "This command will be executed after building target c2play"
            )
        install(TARGETS c2play RUNTIME DESTINATION bin/${OS}/${ARCH}/c2client)
        #install(TARGETS c2play RUNTIME DESTINATION bin/${OS}/${ARCH}/c2server)
    else()
        add_custom_command(TARGET c2play POST_BUILD
            COMMAND bash auto_copy.sh
            WORKING_DIRECTORY ${CORE_PLY_DIR}
            COMMENT "This command will be executed after building target c2play"
            ) 
        install(TARGETS c2play LIBRARY DESTINATION bin/${OS}/${ARCH}/c2client)
        #install(TARGETS c2play LIBRARY DESTINATION bin/${OS}/${ARCH}/c2server)
    endif()

endif()

if(BUILD_STATIC_LIBS)
    if(MSVC)
        add_definitions(-DCM_STATICLIB)
    endif()

    link_libraries(c2com_static)

    add_library(c2play_static STATIC ${SOURCES})

    set_target_properties(c2play_static PROPERTIES OUTPUT_NAME "c2play")

    install(TARGETS c2play_static
        RUNTIME DESTINATION bin/${OS}/${ARCH}
        LIBRARY DESTINATION lib/${OS}/${ARCH}
        ARCHIVE DESTINATION lib/${OS}/${ARCH}
        )
endif()

install(FILES ${EXP_HEADERS} DESTINATION include/c2sdk)
