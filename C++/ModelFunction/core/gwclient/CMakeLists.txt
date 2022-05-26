cmake_minimum_required(VERSION 2.8)

project(libgwsdk)

file(GLOB_RECURSE PROJECT_HEADERS *.h)
file(GLOB_RECURSE PROJECT_CPPS    *.cpp)

set(SOURCES ${PROJECT_HEADERS} ${PROJECT_CPPS})

source_group_by_dir(SOURCES)

if(MSVC)
    add_definitions(-DGWSDK_EXPORTS)
endif()


set(EXP_HEADERS gwsdk.h)

include_directories(
    .
    ${CORE_INC_DIR}
    ${CORE_COM_DIR}
    ${CORE_PLY_DIR}
    )


link_directories(${CORE_LIB_DIR})

if (MSVC)
  link_libraries(
      debug libcurl${CMAKE_DEBUG_POSTFIX} optimized libcurl
      debug libcares${CMAKE_DEBUG_POSTFIX} optimized libcares
      debug jrtplib${CMAKE_DEBUG_POSTFIX} optimized jrtplib
      ws2_32
      delayimp
      Dnsapi
      Qwave
      wldap32
      crypt32
      )
else()
   link_libraries(
        debug curl${CMAKE_DEBUG_POSTFIX}   optimized curl
        debug ssl${CMAKE_DEBUG_POSTFIX}    optimized ssl
        debug crypto${CMAKE_DEBUG_POSTFIX} optimized crypto
        debug jrtp${CMAKE_DEBUG_POSTFIX} optimized jrtp
        -lresolv
        -ldl
        -lz
        )
endif()

link_libraries(
    debug jsoncpp${CMAKE_DEBUG_POSTFIX}     optimized jsoncpp
    debug stk${CMAKE_DEBUG_POSTFIX}         optimized stk
    debug eXosip2${CMAKE_DEBUG_POSTFIX}     optimized eXosip2
    debug osip2${CMAKE_DEBUG_POSTFIX}       optimized osip2
    debug osipparser2${CMAKE_DEBUG_POSTFIX} optimized osipparser2   
    debug jthread${CMAKE_DEBUG_POSTFIX}     optimized jthread
    debug j2x${CMAKE_DEBUG_POSTFIX}         optimized j2x
    debug log4x${CMAKE_DEBUG_POSTFIX}       optimized log4x
    )

add_definitions(-DCURL_STATICLIB -DSTK_STATICLIB -DCM_STATICLIB -DLOG4X_STATICLIB -DJ2X_STATICLIB)

if(BUILD_SHARED_LIBS)
    link_libraries(c2play c2cap)

    add_library(gwsdk SHARED ${SOURCES})
    
    set_target_properties(gwsdk PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CORE_GWSDK_DIR}/${ARCH}
        LIBRARY_OUTPUT_DIRECTORY ${CORE_GWSDK_DIR}/${ARCH}
        ARCHIVE_OUTPUT_DIRECTORY ${CORE_GWSDK_DIR}/${ARCH}
        )
        
    add_custom_command(TARGET gwsdk POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/gwsdk.h ${SDK_EXP_INC_DIR}
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/gwsdk.h ${CORE_INC_DIR}
        COMMENT "This command will be executed after building target gwsdk")

    install(TARGETS gwsdk
        RUNTIME DESTINATION lib/${OS}/${ARCH}
        LIBRARY DESTINATION lib/${OS}/${ARCH}
        ARCHIVE DESTINATION lib/${OS}/${ARCH}
        )
        
    if(MSVC)
        add_custom_command(TARGET gwsdk POST_BUILD COMMAND auto_copy.bat WORKING_DIRECTORY ${CORE_GWSDK_DIR})
        install(TARGETS gwsdk RUNTIME DESTINATION bin/${OS}/${ARCH}/c2client)
        install(TARGETS gwsdk RUNTIME DESTINATION bin/${OS}/${ARCH}/c2server)
    else()
        add_custom_command(TARGET gwsdk POST_BUILD COMMAND bash auto_copy.sh WORKING_DIRECTORY ${CORE_GWSDK_DIR})
        install(TARGETS gwsdk LIBRARY DESTINATION bin/${OS}/${ARCH}/c2client)
        install(TARGETS gwsdk LIBRARY DESTINATION bin/${OS}/${ARCH}/c2server)
    endif()
        
endif()

if(BUILD_STATIC_LIBS)
    if(MSVC)
        add_definitions(-DPLAY_STATICLIB)
    endif()

    link_libraries(c2play_static c2play_static)

    add_library(gwsdk_static STATIC ${SOURCES})
    set_target_properties(gwsdk_static PROPERTIES OUTPUT_NAME "gwsdk")

    add_custom_command(TARGET gwsdk_static POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/c2sdk.h ${SDK_EXP_INC_DIR}
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/c2sdk.h ${CORE_INC_DIR}
        )

    install(TARGETS gwsdk_static
        RUNTIME DESTINATION bin/${OS}/${ARCH}
        LIBRARY DESTINATION lib/${OS}/${ARCH}
        ARCHIVE DESTINATION lib/${OS}/${ARCH}
        )
endif()


