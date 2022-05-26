cmake_minimum_required(VERSION 3.0)

project(libgb28181sdk)

file(GLOB_RECURSE PROJECT_HEADERS *.h)
file(GLOB_RECURSE PROJECT_CPPS    *.cpp)

set(SOURCES ${PROJECT_HEADERS} ${PROJECT_CPPS})

source_group_by_dir(SOURCES)

if(MSVC)
    add_definitions(-DGB28181_EXPORTS)
else()
    add_definitions(-Wno-deprecated-declarations)
endif()

set(EXP_HEADERS	gb28181sdk.h)

include_directories(
    ${CORE_INC_DIR}
    ${CORE_COM_DIR}
    ${CORE_PLY_DIR}
    ${COM_DIR}
    .
    )
link_directories(
    ${CORE_LIB_DIR}
    ${CORE_PSPROCESS_DIR}/${ARCH}
    )

link_directories(
    ${CORE_LIB_DIR}
    ${CORE_LIB_DIR}/3rd/hik
    ${CORE_LIB_DIR}/3rd/tindy
    ${CORE_LIB_DIR}/3rd/dahua
    ${CORE_LIB_DIR}/3rd/helmet
    )
	
if(MSVC)
    link_libraries(
        Dnsapi
        ws2_32
        Qwave
        delayimp
        wldap32
        crypt32
        debug libcares${CMAKE_DEBUG_POSTFIX}    optimized libcares
        debug jrtplib${CMAKE_DEBUG_POSTFIX}     optimized jrtplib
        debug libcurl${CMAKE_DEBUG_POSTFIX}     optimized libcurl
        debug eXosip2${CMAKE_DEBUG_POSTFIX}     optimized eXosip2
        debug osip2${CMAKE_DEBUG_POSTFIX}       optimized osip2
        debug osipparser2${CMAKE_DEBUG_POSTFIX} optimized osipparser2
        debug jthread${CMAKE_DEBUG_POSTFIX}     optimized jthread
        debug j2x${CMAKE_DEBUG_POSTFIX}         optimized j2x
        debug log4x${CMAKE_DEBUG_POSTFIX}       optimized log4x
        psprocess
        )
elseif(ANDROID)
    link_libraries(
        debug curl${CMAKE_DEBUG_POSTFIX}        optimized curl
        debug eXosip2${CMAKE_DEBUG_POSTFIX}     optimized eXosip2
        debug osip2${CMAKE_DEBUG_POSTFIX}       optimized osip2
        debug osipparser2${CMAKE_DEBUG_POSTFIX} optimized osipparser2
        debug jrtp${CMAKE_DEBUG_POSTFIX}        optimized jrtp
        debug jthread${CMAKE_DEBUG_POSTFIX}     optimized jthread
        debug j2x${CMAKE_DEBUG_POSTFIX}         optimized j2x
        debug log4x${CMAKE_DEBUG_POSTFIX}       optimized log4x
        -ldl
        -lz
        )
else()
    link_libraries(
        debug curl${CMAKE_DEBUG_POSTFIX}        optimized curl
        debug ssl${CMAKE_DEBUG_POSTFIX}         optimized ssl
        debug crypto${CMAKE_DEBUG_POSTFIX}      optimized crypto
        debug jrtp${CMAKE_DEBUG_POSTFIX}        optimized jrtp
        debug eXosip2${CMAKE_DEBUG_POSTFIX}     optimized eXosip2
        debug osip2${CMAKE_DEBUG_POSTFIX}       optimized osip2
        debug osipparser2${CMAKE_DEBUG_POSTFIX} optimized osipparser2
        debug jthread${CMAKE_DEBUG_POSTFIX}     optimized jthread
        debug j2x${CMAKE_DEBUG_POSTFIX}         optimized j2x
        debug log4x${CMAKE_DEBUG_POSTFIX}       optimized log4x
        psprocess
        -lresolv
        -ldl
        -lz
        )
endif()


add_definitions(-DCURL_STATICLIB -DSTK_STATICLIB -DJ2X_STATICLIB -DDLL_EXPORT)

if(BUILD_SHARED_LIBS)
    if(MSVC)
        set(CMAKE_SHARED_LINKER_FLAGS_DEBUG   "${CMAKE_SHARED_LINKER_FLAGS_DEBUG}   /NODEFAULTLIB:libcmtd.lib")
        set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /NODEFAULTLIB:libcmt.lib")
    endif()

    link_libraries(c2play c2cap c2fplay)
    add_library(gb28181sdk SHARED ${SOURCES})
    set_target_properties(gb28181sdk PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CORE_GB28181SDK_DIR}/${ARCH}
        LIBRARY_OUTPUT_DIRECTORY ${CORE_GB28181SDK_DIR}/${ARCH}
        ARCHIVE_OUTPUT_DIRECTORY ${CORE_GB28181SDK_DIR}/${ARCH}
        )

    install(TARGETS gb28181sdk
        RUNTIME DESTINATION lib/${OS}/${ARCH}
        LIBRARY DESTINATION lib/${OS}/${ARCH}
        ARCHIVE DESTINATION lib/${OS}/${ARCH}
        )

    if(MSVC)
        add_custom_command(TARGET gb28181sdk
            POST_BUILD
            COMMAND auto_copy.bat
            WORKING_DIRECTORY ${CORE_GB28181SDK_DIR}
            COMMENT "This command will be executed after building target gb28181sdk"
            )
        install(TARGETS gb28181sdk RUNTIME DESTINATION bin/${OS}/${ARCH}/c2client)
    else()
        add_custom_command(TARGET gb28181sdk 
			POST_BUILD 
			COMMAND bash auto_copy.sh 
			WORKING_DIRECTORY ${CORE_GB28181SDK_DIR}
            COMMENT "This command will be executed after building target gb28181sdk"
			)
        install(TARGETS gb28181sdk LIBRARY DESTINATION bin/${OS}/${ARCH}/c2client)
    endif()

endif()

if(BUILD_STATIC_LIBS)
    if(MSVC)
        add_definitions(-DCM_STATICLIB -DSDK_STATICLIB)
    endif()


    link_libraries(psprocess_static)
    add_library(gb28181sdk_static STATIC ${SOURCES})

    set_target_properties(gb28181sdk_static PROPERTIES OUTPUT_NAME "gb28181sdk")

    install(TARGETS gb28181sdk_static
        RUNTIME DESTINATION bin/${OS}/${ARCH}
        LIBRARY DESTINATION lib/${OS}/${ARCH}
        ARCHIVE DESTINATION lib/${OS}/${ARCH}
        )
endif()




install(FILES ${EXP_HEADERS} DESTINATION include/gb28181sdk)
