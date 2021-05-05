# Create a library called "YoctoLib" which includes the .cpp and .c source files and


FILE(GLOB YoctoLibCppSources *.cpp)
FILE(GLOB YoctoLibHSources *.h)

add_library (YoctoLib ${YoctoLibCppSources} ${YoctoLibHSources})

# add yapi low level api
add_subdirectory(yapi)

target_link_libraries(YoctoLib PUBLIC yapi)

# Make sure the compiler can find include files for our library
# when other libraries or executables link to Hello
target_include_directories (YoctoLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
