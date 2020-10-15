# Create a library called "YoctoLib" which includes the .cpp and .c source files and
FILE(GLOB YoctoLibCppSources *.cpp)
FILE(GLOB YoctoLibHSources *.h)
FILE(GLOB YoctoLibCSources yapi/*.c)
add_library (YoctoLib ${YoctoLibCppSources} ${YoctoLibHSources} ${YoctoLibCSources})

# Make sure the compiler can find include files for our Hello library
# when other libraries or executables link to Hello
target_include_directories (YoctoLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# add pthread usb-1.0 library only on linux
if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
target_link_libraries (YoctoLib LINK_PUBLIC pthread usb-1.0)
endif()