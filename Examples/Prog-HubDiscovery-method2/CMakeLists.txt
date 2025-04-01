# CMakeList.txt : CMake project for Prog-CMake, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project(Prog-HubDiscovery-method2)

add_subdirectory(../../Sources yoctolib_bin)

# Add source to this project's executable.
add_executable (Prog-HubDiscovery-method2 "main.cpp")

target_link_libraries(Prog-HubDiscovery-method2 PUBLIC YoctoLib)
