# CMakeList.txt : CMake project for Prog-CMake, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project(Doc-GettingStarted-Yocto-MaxiBuzzer)

add_subdirectory(../../Sources yoctolib_bin)

# Add source to this project's executable.
add_executable (Doc-GettingStarted-Yocto-MaxiBuzzer "main.cpp")

target_link_libraries(Doc-GettingStarted-Yocto-MaxiBuzzer PUBLIC YoctoLib)
