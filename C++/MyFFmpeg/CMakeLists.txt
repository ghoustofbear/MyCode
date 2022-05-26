cmake_minimum_required(VERSION 3.10)
project(FFmpeg_Function)

set(MYFFMPEG_INC_DIR ${CORE_INC_DIR}/MyFFmpeg)

file(GLOB MYFFMPEG_H ${MYFFMPEG_INC_DIR}/*.h)
file(GLOB MYFFMPEG_CPPS ./src/MyFFmpeg/*.cpp)
file(GLOB LIBAVCODEC_H ${MYFFMPEG_INC_DIR}/libavcodec/*.h)
file(GLOB LIBAVDEVICE_H ${MYFFMPEG_INC_DIR}/libavdevice/*.h)
file(GLOB LIBAVFILTER_H ${MYFFMPEG_INC_DIR}/libavfilter/*.h)
file(GLOB LIBAVFORMAT_H ${MYFFMPEG_INC_DIR}/libavformat/*.h)
file(GLOB LIBAVUTIL_H ${MYFFMPEG_INC_DIR}/libavutil/*.h)
file(GLOB LIBPOSTPROC_H ${MYFFMPEG_INC_DIR}/libpostproc/*.h)
file(GLOB LIBSWRESAMPLE_H ${MYFFMPEG_INC_DIR}/libswresample/*.h)
file(GLOB LIBSWSCALE_H ${MYFFMPEG_INC_DIR}/libswscale/*.h)
file(GLOB SDL_H ./include/x64/SDL2/*.h)


set(CORE_INC_DIR 
   ./include/x64
   ./include/x64/MyFFmpeg
    )

message(STATUS,"CORE_INC_DIR is ${CORE_INC_DIR}")
set(CORE_LIB_DIR 
    ./libs/x64)
message(STATUS,"CORE_LIB_DIR is ${CORE_LIB_DIR}")
include_directories(${CORE_INC_DIR})
link_directories(${CORE_LIB_DIR})
set(MY_FFMPEG_PATH ./src/MyFFmpeg)
set(MyFFMPEG_SDL_PATH ./src/MyFFmpegSDL)
add_subdirectory(${MY_FFMPEG_PATH})
add_subdirectory(${MyFFMPEG_SDL_PATH})
