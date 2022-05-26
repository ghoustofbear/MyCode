cmake_minimum_required(VERSION 3.0)

project(libc2com)

include_directories(${CORE_INC_DIR})

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/concurrency.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tls_error.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/stream.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/thread.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utility.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/socket.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sys.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/mempool.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/md5.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/initializer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/fsrw.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/base64_ex.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dog.cpp
    )

if(MSVC)
    list(APPEND SOURCES 
        ${CMAKE_CURRENT_SOURCE_DIR}/wsys.cpp 
        ${CMAKE_CURRENT_SOURCE_DIR}/wsocket.cpp)
    list(APPEND SOURCES 
        ${CMAKE_CURRENT_SOURCE_DIR}/c2com.rc)
else()
    list(APPEND SOURCES 
        ${CMAKE_CURRENT_SOURCE_DIR}/usys.cpp 
        ${CMAKE_CURRENT_SOURCE_DIR}/usocket.cpp 
        ${CMAKE_CURRENT_SOURCE_DIR}/uuid.cpp)
endif()



source_group_by_dir(SOURCES)

if(MSVC)
    add_definitions(-DCM_EXPORTS)
    add_definitions(-wd"4819")
    link_libraries(ws2_32 Iphlpapi)
else()
    add_definitions(-Wno-deprecated-declarations)
endif()

link_directories(${CORE_LIB_DIR})


link_libraries(debug log4x${CMAKE_DEBUG_POSTFIX} optimized log4x)

if(ANDROID)
    link_libraries(
    -llog
    )
endif()

add_definitions(-DLOG4X_STATICLIB)

if(BUILD_SHARED_LIBS)
    add_library(c2com SHARED ${SOURCES} ${HEADERS})
    set_target_properties(c2com PROPERTIES 
        RUNTIME_OUTPUT_DIRECTORY ${CORE_COM_DIR}/${ARCH}
        LIBRARY_OUTPUT_DIRECTORY ${CORE_COM_DIR}/${ARCH}
        ARCHIVE_OUTPUT_DIRECTORY ${CORE_COM_DIR}/${ARCH}
        )

    install(TARGETS c2com
        RUNTIME DESTINATION lib/${OS}/${ARCH}
        LIBRARY DESTINATION lib/${OS}/${ARCH}
        ARCHIVE DESTINATION lib/${OS}/${ARCH}
        )

    if(MSVC)
        add_custom_command(TARGET c2com
            POST_BUILD
            COMMAND auto_copy.bat
            WORKING_DIRECTORY ${CORE_COM_DIR}
            COMMENT "This command will be executed after building target c2com"
            )
        install(TARGETS c2com RUNTIME DESTINATION bin/${OS}/${ARCH}/c2client)
        install(TARGETS c2com RUNTIME DESTINATION bin/${OS}/${ARCH}/c2server)
    else()
        add_custom_command(TARGET c2com
            POST_BUILD
            COMMAND bash auto_copy.sh
            WORKING_DIRECTORY ${CORE_COM_DIR}
            COMMENT "This command will be executed after building target c2com"
            )
        install(TARGETS c2com LIBRARY DESTINATION bin/${OS}/${ARCH}/c2client)
        install(TARGETS c2com LIBRARY DESTINATION bin/${OS}/${ARCH}/c2server)
    endif()

endif()

if(BUILD_STATIC_LIBS)
    add_library(c2com_static STATIC ${SOURCES} ${HEADERS})

    set_target_properties(c2com_static PROPERTIES OUTPUT_NAME "c2com")

    install(TARGETS c2com_static
        RUNTIME DESTINATION bin/${OS}/${ARCH}
        LIBRARY DESTINATION lib/${OS}/${ARCH}
        ARCHIVE DESTINATION lib/${OS}/${ARCH}
        )
endif()

# install(FILES ${HEADERS} DESTINATION include)
