cmake_minimum_required(VERSION 2.8)

project(libpsprocess)

aux_source_directory(. SOURCES)
include_directories(${STK_INC_DIR}  ${CORE_COM_DIR}   ${CORE_INC_DIR})

set(HEADERS
    PsProcessApi.h
	PsPacker.h
	ps_parser.h
	PsProcess.h
    )

link_directories(
    ${CORE_LIB_DIR}
    )

link_libraries(c2com)

if(MSVC)
    add_definitions(-DPSPROCESS_EXPORTS)
endif()


if(BUILD_SHARED_LIBS)
    add_library(psprocess SHARED ${SOURCES} ${HEADERS})

	set_target_properties(psprocess PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CORE_PSPROCESS_DIR}/${ARCH}
        LIBRARY_OUTPUT_DIRECTORY ${CORE_PSPROCESS_DIR}/${ARCH}
        ARCHIVE_OUTPUT_DIRECTORY ${CORE_PSPROCESS_DIR}/${ARCH}
        )

	install(TARGETS psprocess
        RUNTIME DESTINATION lib/${OS}/${ARCH}
        LIBRARY DESTINATION lib/${OS}/${ARCH}
        ARCHIVE DESTINATION lib/${OS}/${ARCH}
        )
	  if(MSVC)
        add_custom_command(TARGET psprocess POST_BUILD COMMAND auto_copy.bat WORKING_DIRECTORY ${CORE_PSPROCESS_DIR})
		install(TARGETS psprocess RUNTIME DESTINATION bin/${OS}/${ARCH}/c2client)
        install(TARGETS psprocess RUNTIME DESTINATION bin/${OS}/${ARCH}/c2server)
    else()
        add_custom_command(TARGET psprocess POST_BUILD COMMAND bash auto_copy.sh WORKING_DIRECTORY ${CORE_PSPROCESS_DIR})
		install(TARGETS psprocess LIBRARY DESTINATION bin/${OS}/${ARCH}/c2client)
        install(TARGETS psprocess LIBRARY DESTINATION bin/${OS}/${ARCH}/c2server)
    endif()
endif()

if(BUILD_STATIC_LIBS)
    add_library(psprocess_static STATIC ${SOURCES} ${HEADERS})
    set_target_properties(psprocess_static PROPERTIES OUTPUT_NAME "psprocess")
    add_custom_command(TARGET psprocess_static POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/PsProcessApi.h ${SDK_EXP_INC_DIR}
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/PsProcessApi.h ${CORE_INC_DIR}
        )
    install(TARGETS psprocess_static
        RUNTIME DESTINATION bin/${OS}/${ARCH}
        LIBRARY DESTINATION lib/${OS}/${ARCH}
        ARCHIVE DESTINATION lib/${OS}/${ARCH}
        )
endif()

install(FILES ${HEADERS} DESTINATION include/psprocess)
