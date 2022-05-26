cmake_minimum_required(VERSION 3.0)

project(libUtNet)

file(GLOB_RECURSE PROJECT_HEADERS *.h)
file(GLOB_RECURSE PROJECT_CPPS    *.cpp)

set(SOURCES ${PROJECT_HEADERS} ${PROJECT_CPPS})

source_group_by_dir(SOURCES)

if(MSVC)
    list(APPEND SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/UtNet.rc)
endif()

set(EXP_HEADERS ut_net.h)

include_directories(
    ${CORE_INC_DIR}
    #${CORE_COM_DIR}
    #${CORE_SDK_DIR}
    )


link_directories(
    ${CORE_PSPROCESS_DIR}/${ARCH}
	${CORE_LIB_DIR}
    )
	


add_definitions(-DCURL_STATICLIB -DHTTP_ONLY -DSTK_STATICLIB -DCONFIG_DLL)

if(WIN32)
add_definitions(-DNET_EXPORTS -wd"4819")
add_definitions(-DDLL_EXPORT)

link_directories(${CORE_LIB_DIR}/3rd/helmet
                 ${CORE_LIB_DIR}/3rd/hik
                 ${CORE_LIB_DIR}/3rd/dahua
                 ${CORE_LIB_DIR}/3rd/tindy
				 ${CORE_LIB_DIR}/utNet)

link_libraries(debug c2sdk${CMAKE_DEBUG_POSTFIX}   optimized c2sdk
			   debug c2cap${CMAKE_DEBUG_POSTFIX}   optimized c2cap
               debug libcurl${CMAKE_DEBUG_POSTFIX} optimized libcurl
               debug jsoncpp${CMAKE_DEBUG_POSTFIX} optimized jsoncpp
               debug stk${CMAKE_DEBUG_POSTFIX}     optimized stk
               PlayCtrl
               dhplay
               ws2_32
               wldap32
               crypt32
               VARndr
			   HttpUtilLib
			   VideoSDK
			   PlayCtrl
			   PlaySdkM4
			   dhplay
			   RSPlay
			   sqlite3
			   HCNetSDK
			   VideoCapture
			   mxml
			   winmm
			   iphlpapi
			   Mux_AVFile
               )
			   
			   
			   
			   
add_library(UtNet SHARED ${SOURCES})
set_target_properties(UtNet PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY ${CORE_UTNET_DIR}/${ARCH}
	LIBRARY_OUTPUT_DIRECTORY ${CORE_UTNET_DIR}/${ARCH}
	ARCHIVE_OUTPUT_DIRECTORY ${CORE_UTNET_DIR}/${ARCH}
	)



install(TARGETS UtNet
	RUNTIME DESTINATION lib/${OS}/${ARCH}
	LIBRARY DESTINATION lib/${OS}/${ARCH}
	ARCHIVE DESTINATION lib/${OS}/${ARCH}
	)


add_custom_command(TARGET UtNet POST_BUILD COMMAND auto_copy.bat WORKING_DIRECTORY ${CORE_UTNET_DIR})
install(TARGETS UtNet RUNTIME DESTINATION bin/${OS}/${ARCH}/c2client)
install(TARGETS UtNet RUNTIME DESTINATION bin/${OS}/${ARCH}/c2server)

			   

endif()


install(FILES ${EXP_HEADERS} DESTINATION include/ut_net.h)
