cmake_minimum_required(VERSION 3.10)
project(MyFFmpegSDL)


add_executable(${PROJECT_NAME}
                ${LIBAVCODEC_H}
                ${LIBAVDEVICE_H}
                ${LIBAVFILTER_H}
                ${LIBAVFORMAT_H}
                ${LIBSWRESAMPLE_H}
                ${LIBSWSCALE_H}
                ${LIBPOSTPROC_H}
                ${SDL_H}
                 main.cpp               
)           
target_link_libraries(${PROJECT_NAME}
                    avcodec.lib
                    avformat.lib
                    avutil.lib
                    avdevice.lib
                    avfilter.lib
                    postproc.lib
                    swresample.lib
                    swscale.lib
                    SDL2.lib
                    SDL2main.lib
                        )
