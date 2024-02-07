#
# CMake build system design considerations:
#
# - Include directories:
#   + Do not define include directories globally using the include_directories
#     command but rather at the target level using the
#     target_include_directories command. That way, it is easier to guarantee
#     that targets are built using the proper list of include directories.
#   + Use the PUBLIC and PRIVATE keywords to specifiy the scope of include
#     directories. That way, a target linking to a library (using the
#     target_link_librairies command) inherits from the library PUBLIC include
#     directories and not from the PRIVATE ones.
#   + Note: there is currently one remaining include_directories command in the
#     CMake files. It is related to ZLIB support which is planned to be removed.
#     When the support is removed, the associated include_directories command
#     will be removed as well as this note.
# - MBEDTLS_TARGET_PREFIX: CMake targets are designed to be alterable by calling
#   CMake in order to avoid target name clashes, via the use of
#   MBEDTLS_TARGET_PREFIX. The value of this variable is prefixed to the
#   mbedtls, mbedx509, mbedcrypto and apidoc targets.
#


# Set the project root directory.
set(MBEDTLS_DIR ${CMAKE_CURRENT_SOURCE_DIR})


add_subdirectory(include)

add_subdirectory(library)
