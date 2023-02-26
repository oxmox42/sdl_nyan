cmake_minimum_required(VERSION 3.14)

project(sdl-nyan)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
## Create binaries in the root of the build directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "ON")

find_package(SDL2 REQUIRED)

add_subdirectory(src)

