# CMakeList.txt : CMake project for Prog-CMake, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project(Doc-GettingStarted-Yocto-Display)

add_subdirectory(../../Sources yoctolib_bin)

# Add source to this project's executable.
add_executable (Doc-GettingStarted-Yocto-Display "main.cpp")

target_link_libraries(Doc-GettingStarted-Yocto-Display PUBLIC YoctoLib)
