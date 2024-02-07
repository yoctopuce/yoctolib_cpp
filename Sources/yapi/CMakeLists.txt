cmake_minimum_required(VERSION 3.8)

project(yapi VERSION 1.0.1 DESCRIPTION "Yoctopuce YAPI V2 library")

option(USE_YSSL "Include SSL/TLS support" ON)

if(USE_YSSL)
add_subdirectory(mbedtls)
else()
add_compile_definitions(NO_YSSL)
endif()

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT AND ${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
SET(CMAKE_INSTALL_PREFIX "/usr")
endif()
include(GNUInstallDirs)


set(YAPI_C_FILES
	yapi.c
	ystream.c
	yprog.c
	yfifo.c
	ykey.c
	yhash.c
	yjson.c
	ytcp.c
	ymemory.c
	ythread.c
	yjni.c
	ypkt_win.c
	ypkt_osx.c
	ypkt_lin.c
	yssl.c
)

add_library (yapi  ${YAPI_C_FILES})
if(USE_YSSL)
	target_include_directories(yapi  PRIVATE mbedtls/include)
	target_link_libraries(yapi mbedtls)
endif()


target_include_directories (yapi PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# add pthread usb-1.0 library only on linux
if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
target_link_libraries (yapi LINK_PUBLIC pthread usb-1.0)
endif()

if (APPLE)
	find_library(CORE_FRAMEWORK CoreFoundation)
	if (NOT CORE_FRAMEWORK)
	    message(FATAL_ERROR "CoreFoundation not found")
	endif()
	find_library(IOKIT_FRAMEWORK IOKit)
	if (NOT IOKIT_FRAMEWORK)
	    message(FATAL_ERROR "IOKit not found")
	endif()
	target_link_libraries(yapi ${IOKIT_FRAMEWORK} ${CORE_FRAMEWORK})
endif()

set_target_properties(yapi PROPERTIES SOVERSION 1)
install(TARGETS yapi DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(FILES yapi.h ydef.h yversion.h yjson.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/yapi)


