cmake_minimum_required(VERSION 3.0)

project(libc2fplay)

file(GLOB_RECURSE PROJECT_HEADERS *.h)
file(GLOB_RECURSE PROJECT_CPPS    *.cpp)

set(SOURCES ${PROJECT_HEADERS} ${PROJECT_CPPS})

if(MSVC)
    list(APPEND SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/c2fplay.rc)
endif()


if(ANDROID)
    list(REMOVE_ITEM SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/tindy_player.h )
    list(REMOVE_ITEM SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/tindy_player.cpp )
    list(REMOVE_ITEM SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/hik_player.h )
    list(REMOVE_ITEM SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/hik_player.cpp )
    list(REMOVE_ITEM SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/dahua_player.h )
    list(REMOVE_ITEM SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/dahua_player.cpp )
    list(REMOVE_ITEM SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/helmet_player.h )
    list(REMOVE_ITEM SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/helmet_player.cpp)
elseif(UNIX)
    list(REMOVE_ITEM SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/tindy_player.h )
    list(REMOVE_ITEM SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/tindy_player.cpp)
    list(REMOVE_ITEM SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/helmet_player.h )
    list(REMOVE_ITEM SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/helmet_player.cpp)
endif()

source_group_by_dir(SOURCES)

set(EXP_HEADERS c2fplay.h playerlistener.h)

include_directories(
    ${CORE_SDK_DIR}
    ${CORE_INC_DIR}
    ${CORE_COM_DIR}
    ${CORE_PLY_DIR}
    )


include_directories(
	${CORE_PSPROCESS_DIR}
    )
link_directories(
    ${CORE_PSPROCESS_DIR}/${ARCH}
    )
link_libraries(
 debug psprocess${CMAKE_DEBUG_POSTFIX}         optimized psprocess
    )


if(MSVC)
    add_definitions(-DFPLY_EXPORTS)
    add_definitions(-wd"4819")
else(NOT MSVC)
    add_definitions(-Wno-deprecated-declarations)
endif()

link_directories(${CORE_LIB_DIR} ${CORE_LIB_DIR}/3rd/hik)
link_directories(${CORE_LIB_DIR} ${CORE_LIB_DIR}/3rd/tindy)
link_directories(${CORE_LIB_DIR} ${CORE_LIB_DIR}/3rd/dahua)
link_directories(${CORE_LIB_DIR} ${CORE_LIB_DIR}/3rd/helmet)

if(MSVC)
    link_libraries(VARndr)
else()
    link_libraries(
        AudioRender
        SuperRender
        )
endif()

link_libraries(PlayCtrl dhplay)

#add_definitions(-DCURL_STATICLIB -DSTK_STATICLIB)

if(BUILD_SHARED_LIBS)
    link_libraries(c2com)
    #link_libraries(c2play)

    add_library(c2fplay SHARED ${SOURCES})
    set_target_properties(c2fplay PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CORE_FPLY_DIR}/${ARCH}
        LIBRARY_OUTPUT_DIRECTORY ${CORE_FPLY_DIR}/${ARCH}
        ARCHIVE_OUTPUT_DIRECTORY ${CORE_FPLY_DIR}/${ARCH}
        )

    add_custom_command(TARGET c2fplay POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/c2fplay.h ${SDK_EXP_INC_DIR}
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/c2fplay.h ${CORE_INC_DIR}
        COMMENT "This command will be executed after building target c2fplay")

    install(TARGETS c2fplay
        RUNTIME DESTINATION lib/${OS}/${ARCH}
        LIBRARY DESTINATION lib/${OS}/${ARCH}
        ARCHIVE DESTINATION lib/${OS}/${ARCH}
        )

    if(MSVC)
        add_custom_command(TARGET c2fplay POST_BUILD COMMAND auto_copy.bat WORKING_DIRECTORY ${CORE_FPLY_DIR})
        install(TARGETS c2fplay RUNTIME DESTINATION bin/${OS}/${ARCH}/c2client)
        install(TARGETS c2fplay RUNTIME DESTINATION bin/${OS}/${ARCH}/c2server)
    else()
        add_custom_command(TARGET c2fplay POST_BUILD COMMAND bash auto_copy.sh WORKING_DIRECTORY ${CORE_FPLY_DIR})
        install(TARGETS c2fplay LIBRARY DESTINATION bin/${OS}/${ARCH}/c2client)
        install(TARGETS c2fplay LIBRARY DESTINATION bin/${OS}/${ARCH}/c2server)
    endif()

endif()

if(BUILD_STATIC_LIBS)
    if(MSVC)
        add_definitions(-DPLAY_STATICLIB -DCM_STATICLIB)
    endif()

    link_libraries(c2play_static c2com_static)

    add_library(c2fplay_static STATIC ${SOURCES})
    set_target_properties(c2fplay_static PROPERTIES OUTPUT_NAME "c2fplay")

    add_custom_command(TARGET c2fplay_static POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/c2fplay.h ${SDK_EXP_INC_DIR}
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/c2fplay.h ${CORE_INC_DIR}
        )

    install(TARGETS c2fplay_static
        RUNTIME DESTINATION bin/${OS}/${ARCH}
        LIBRARY DESTINATION lib/${OS}/${ARCH}
        ARCHIVE DESTINATION lib/${OS}/${ARCH}
        )
endif()

install(FILES ${EXP_HEADERS} DESTINATION include/c2fplay)
