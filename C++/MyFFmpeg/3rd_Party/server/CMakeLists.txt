cmake_minimum_required(VERSION 3.0)

project(libc2msv)

file(GLOB_RECURSE PROJECT_HEADERS *.h)
file(GLOB_RECURSE PROJECT_CPPS    *.cpp)

set(SOURCES ${PROJECT_HEADERS} ${PROJECT_CPPS})

if(MSVC)
    list(APPEND SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/c2msv.rc)
endif()

list(REMOVE_ITEM SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/pu/hs_pu.cpp)
list(REMOVE_ITEM SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/pu/hs_pu.h)

source_group_by_dir(SOURCES)

set(EXP_HEADERS
    config/observer.h
    config/observable.h
    config/cfg_channel.h
    config/cfg_group.h
    config/cfg_platform.h
    config/cfg_sup_platform.h
    config/cfg_user_group.h
    config/cfg_device.h
    config/cfg_pub_info.h
    config/cfg.h
    config/cfg_sub_platform.h
    config/cfg_system.h
    config/cfg_alarm_log.h
    config/cfg_device_log.h
    config/cfg_system_log.h
    config/cfg_user_log.h
    config/cfg_server.h
    config/cfg_user.h
    config/view.h
    config/view_user_group.h
	config/cfg_catalog_info.h
    exports.h
    servmgr.h
    log.h
    onuiupdate.h
    )

include_directories(
    ${CORE_INC_DIR}
    ${CORE_COM_DIR}
    ${CORE_PLY_DIR}
    ${COM_DIR}
    .
    config
    gw
    pu
    auth
    private
    websocket
    xml
    openapi
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
    add_definitions(-DMS_EXPORTS)
    add_definitions(-wd"4819")
else()
    add_definitions(-Wno-deprecated-declarations)
endif()

link_directories(
    ${CORE_LIB_DIR}
    ${CORE_LIB_DIR}/3rd/hik
    ${CORE_LIB_DIR}/3rd/tindy
    ${CORE_LIB_DIR}/3rd/dahua
    ${CORE_LIB_DIR}/3rd/helmet
	${CORE_LIB_DIR}/3rd/hikehome
    )

if(MSVC)
    link_libraries(
        debug libcurl${CMAKE_DEBUG_POSTFIX}   optimized libcurl
        ws2_32
        delayimp
        Qwave
        Dnsapi
        debug libcares${CMAKE_DEBUG_POSTFIX}  optimized libcares
        debug jrtplib${CMAKE_DEBUG_POSTFIX}   optimized jrtplib
        debug utivasdk${CMAKE_DEBUG_POSTFIX}  optimized utivasdk
        )
else()
    link_libraries(
        -lresolv
        -Wl,-rpath=${CORE_LIB_DIR}/3rd/hik/
        -Wl,-rpath=${CORE_LIB_DIR}/3rd/tindy/
        -Wl,-rpath=${CORE_LIB_DIR}/3rd/dahua/
        -Wl,-rpath=${CORE_LIB_DIR}/3rd/helmet/
        debug jrtp${CMAKE_DEBUG_POSTFIX}      optimized jrtp
        debug curl${CMAKE_DEBUG_POSTFIX}      optimized curl
        debug utivasdkd optimized utivasdk
        )
endif()

link_libraries(
    debug jsoncpp${CMAKE_DEBUG_POSTFIX}     optimized jsoncpp
    debug stk${CMAKE_DEBUG_POSTFIX}         optimized stk
    debug event${CMAKE_DEBUG_POSTFIX}       optimized event
    debug eXosip2${CMAKE_DEBUG_POSTFIX}     optimized eXosip2
    debug osip2${CMAKE_DEBUG_POSTFIX}       optimized osip2
    debug osipparser2${CMAKE_DEBUG_POSTFIX} optimized osipparser2
    debug jthread${CMAKE_DEBUG_POSTFIX}     optimized jthread
    debug easysqlite${CMAKE_DEBUG_POSTFIX}  optimized easysqlite
    debug j2x${CMAKE_DEBUG_POSTFIX}         optimized j2x
    hcnetsdk
    nvssdk
    dhconfigsdk
    dhnetsdk
    iCVSC2
    iCVSFrameAnalyzer
	HCISUPCMS
	HCISUPStream
    )


if(NOT MSVC)
    link_libraries(
        debug event_pthreads${CMAKE_DEBUG_POSTFIX} optimized event_pthreads
        -lresolv)
endif()

add_definitions(-DCURL_STATICLIB -DSTK_STATICLIB -DJ2X_STATICLIB -DDLL_EXPORT -DUSING_EVTHREAD)

if(BUILD_SHARED_LIBS)
    if(MSVC)
        #set(CMAKE_SHARED_LINKER_FLAGS_DEBUG   "${CMAKE_SHARED_LINKER_FLAGS_DEBUG}   /NODEFAULTLIB:libcmtd.lib")
        #set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /NODEFAULTLIB:libcmt.lib")
    endif()

    #link_libraries(c2com)
    link_libraries(c2sdk)

    add_library(c2msv SHARED ${SOURCES})
    set_target_properties(c2msv PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CORE_MSV_DIR}/${ARCH}
        LIBRARY_OUTPUT_DIRECTORY ${CORE_MSV_DIR}/${ARCH}
        ARCHIVE_OUTPUT_DIRECTORY ${CORE_MSV_DIR}/${ARCH}
        )

    install(TARGETS c2msv
        RUNTIME DESTINATION lib/${OS}/${ARCH}
        LIBRARY DESTINATION lib/${OS}/${ARCH}
        ARCHIVE DESTINATION lib/${OS}/${ARCH}
        )

    if(MSVC)
        add_custom_command(TARGET c2msv
            POST_BUILD
            COMMAND auto_copy.bat
            WORKING_DIRECTORY ${CORE_MSV_DIR}
            COMMENT "This command will be executed after building target c2msv"
            )
        install(TARGETS c2msv RUNTIME DESTINATION bin/${OS}/${ARCH}/c2server)
    else()
        add_custom_command(TARGET c2msv POST_BUILD
        COMMAND bash ./auto_copy.sh
        #COMMAND ${CMAKE_COMMAND} -E copy ${CORE_LIB_DIR}/libeasysqlite${CMAKE_DEBUG_POSTFIX}.so ${CORE_MSV_DIR}/${ARCH}
        WORKING_DIRECTORY ${CORE_MSV_DIR}
        COMMENT "This command will be executed after building target c2msv")

        install(TARGETS c2msv LIBRARY DESTINATION bin/${OS}/${ARCH}/c2server)
    endif()

endif()

if(BUILD_STATIC_LIBS)
    if(MSVC)
        add_definitions(-DCM_STATICLIB -DSDK_STATICLIB)
    endif()

    #link_libraries(c2com_static)
    link_libraries(c2sdk_static)

    add_library(c2msv_static STATIC ${SOURCES})

    set_target_properties(c2msv_static PROPERTIES OUTPUT_NAME "c2msv")

    install(TARGETS c2msv_static
        RUNTIME DESTINATION bin/${OS}/${ARCH}
        LIBRARY DESTINATION lib/${OS}/${ARCH}
        ARCHIVE DESTINATION lib/${OS}/${ARCH}
        )
endif()




install(FILES ${EXP_HEADERS} DESTINATION include/c2msv)
