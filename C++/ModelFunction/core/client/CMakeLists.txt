cmake_minimum_required(VERSION 3.0)

project(libc2sdk)

file(GLOB_RECURSE PROJECT_HEADERS *.h)
file(GLOB_RECURSE PROJECT_CPPS    *.cpp)

if(NOT MSVC)
    FOREACH(HEADFILE ${PROJECT_HEADERS})
        if(${HEADFILE} MATCHES  ".*helmet_player.*")
            MESSAGE( "TRUE_FOUND_TINDYPLAYER_H" )
            MESSAGE( ${HEADFILE} )
            list(REMOVE_ITEM PROJECT_HEADERS ${HEADFILE} )
        endif()
    ENDFOREACH()

    FOREACH(SRCFILE ${PROJECT_CPPS})
        if(${SRCFILE} MATCHES ".*helmet_player.*")
            MESSAGE( "TRUE_FOUND_TINDYPLAYER_CPP" )
            MESSAGE( ${SRCFILE} )
            list(REMOVE_ITEM PROJECT_CPPS ${SRCFILE} )
        endif()
    ENDFOREACH()
endif()

set(SOURCES ${PROJECT_HEADERS} ${PROJECT_CPPS})

if(MSVC)
    list(APPEND SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/c2sdk.rc)
endif()

list(REMOVE_ITEM SOURCES ${CORE_FPLY_DIR}/c2fplay.h)
list(REMOVE_ITEM SOURCES ${CORE_FPLY_DIR}/c2fplay.cpp)



set(EXP_HEADERS c2sdk.h c2types.h)

include_directories(
    .
    player
    ${CORE_INC_DIR}
    ${CORE_COM_DIR}
    ${CORE_PLY_DIR}
    )

include_directories(
    ${CORE_PSPROCESS_DIR}
    )
link_directories(${CORE_PSPROCESS_DIR}/${ARCH})


link_directories(${CORE_LIB_DIR})
add_definitions(-DCURL_STATICLIB -DHTTP_ONLY -DSTK_STATICLIB)


if(WIN32)
add_definitions(-DNET_EXPORTS -wd"4819")

link_directories(${CORE_LIB_DIR}/3rd/helmet
                 ${CORE_LIB_DIR}/3rd/hik
                 ${CORE_LIB_DIR}/3rd/dahua
                 ${CORE_LIB_DIR}/3rd/tindy)

link_libraries(psprocess)

link_libraries(debug libcurl${CMAKE_DEBUG_POSTFIX} optimized libcurl
               debug jsoncpp${CMAKE_DEBUG_POSTFIX} optimized jsoncpp
               debug stk${CMAKE_DEBUG_POSTFIX}     optimized stk
               PlayCtrl
               dhplay
               ws2_32
               wldap32
               crypt32
               VARndr
               )

elseif(APPLE)
add_definitions(-Wno-deprecated-declarations)
elseif(ANDROID)


list(REMOVE_ITEM SOURCES ${CORE_FPLY_DIR}/tindy_player.cpp)
list(REMOVE_ITEM SOURCES ${CORE_FPLY_DIR}/hik_player.cpp)
list(REMOVE_ITEM SOURCES ${CORE_FPLY_DIR}/hik_ps_player.cpp)
list(REMOVE_ITEM SOURCES ${CORE_FPLY_DIR}/dahua_player.cpp)
list(REMOVE_ITEM SOURCES ${CORE_FPLY_DIR}/helmet_player.cpp)

add_definitions(-Wno-deprecated-declarations)

link_libraries(
    debug curl${CMAKE_DEBUG_POSTFIX}   optimized curl
    debug ssl${CMAKE_DEBUG_POSTFIX}    optimized ssl
    debug crypto${CMAKE_DEBUG_POSTFIX} optimized crypto
    debug jsoncpp${CMAKE_DEBUG_POSTFIX}.so.19 optimized jsoncpp
    debug stk${CMAKE_DEBUG_POSTFIX}     optimized stk
    -ldl
    -lz
    )

elseif(UNIX)

add_definitions(-Wno-deprecated-declarations)
add_definitions(-DUNIX_DRAW_RECT)

link_directories(${CORE_LIB_DIR}/3rd/helmet
                 ${CORE_LIB_DIR}/3rd/hik
                 ${CORE_LIB_DIR}/3rd/dahua
                 ${CORE_LIB_DIR}/3rd/tindy)

link_libraries(psprocess)
link_libraries(debug curl${CMAKE_DEBUG_POSTFIX}   optimized curl
               debug ssl${CMAKE_DEBUG_POSTFIX}    optimized ssl
               debug crypto${CMAKE_DEBUG_POSTFIX} optimized crypto
               debug jsoncpp${CMAKE_DEBUG_POSTFIX} optimized jsoncpp
               debug stk${CMAKE_DEBUG_POSTFIX}     optimized stk
               PlayCtrl
               dhplay
               AudioRender
               SuperRender
               -ldl
               -lz)

endif()

source_group_by_dir(SOURCES)

if(BUILD_SHARED_LIBS)
    if (ANDROID)
        link_libraries(c2cap c2com)
    else()
        link_libraries(c2cap c2com)
    endif()
    
    add_library(c2sdk SHARED ${SOURCES})
    set_target_properties(c2sdk PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CORE_SDK_DIR}/${ARCH}
        LIBRARY_OUTPUT_DIRECTORY ${CORE_SDK_DIR}/${ARCH}
        ARCHIVE_OUTPUT_DIRECTORY ${CORE_SDK_DIR}/${ARCH}
        )

    add_custom_command(TARGET c2sdk POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/c2sdk.h ${SDK_EXP_INC_DIR}
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/c2sdk.h ${CORE_INC_DIR}
        COMMENT "This command will be executed after building target c2sdk")

    install(TARGETS c2sdk
        RUNTIME DESTINATION lib/${OS}/${ARCH}
        LIBRARY DESTINATION lib/${OS}/${ARCH}
        ARCHIVE DESTINATION lib/${OS}/${ARCH}
        )

    if(MSVC)
        add_custom_command(TARGET c2sdk POST_BUILD COMMAND auto_copy.bat WORKING_DIRECTORY ${CORE_SDK_DIR})
        install(TARGETS c2sdk RUNTIME DESTINATION bin/${OS}/${ARCH}/c2client)
        install(TARGETS c2sdk RUNTIME DESTINATION bin/${OS}/${ARCH}/c2server)
    else()
        add_custom_command(TARGET c2sdk POST_BUILD COMMAND bash auto_copy.sh WORKING_DIRECTORY ${CORE_SDK_DIR})
        install(TARGETS c2sdk LIBRARY DESTINATION bin/${OS}/${ARCH}/c2client)
        install(TARGETS c2sdk LIBRARY DESTINATION bin/${OS}/${ARCH}/c2server)
    endif()

endif()

if(BUILD_STATIC_LIBS)
    if(MSVC)
        add_definitions(-DPLAY_STATICLIB -DCM_STATICLIB)
    endif()

    link_libraries(c2play_static c2com_static)

    add_library(c2sdk_static STATIC ${SOURCES})
    set_target_properties(c2sdk_static PROPERTIES OUTPUT_NAME "c2sdk")

    add_custom_command(TARGET c2sdk_static POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/c2sdk.h ${SDK_EXP_INC_DIR}
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/c2sdk.h ${CORE_INC_DIR}
        )

    install(TARGETS c2sdk_static
        RUNTIME DESTINATION bin/${OS}/${ARCH}
        LIBRARY DESTINATION lib/${OS}/${ARCH}
        ARCHIVE DESTINATION lib/${OS}/${ARCH}
        )
endif()

install(FILES ${EXP_HEADERS} DESTINATION include/c2sdk)
