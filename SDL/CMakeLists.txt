cmake_minimum_required(VERSION 3.25)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

project(pratice_sdl3)

option(BUILD_WARNINGS "add some compiler optoin when build" ON)

include(cmake/fetch_sdl.cmake)
include(cmake/utils.cmake)

fetch_sdl()

add_subdirectory(common/SDL)
add_subdirectory(common/opengl)

add_subdirectory(3rd/glad)
add_subdirectory(3rd/stb)
add_subdirectory(3rd/glm)

add_subdirectory(00-base_framework)
add_subdirectory(01-gpu_render)
add_subdirectory(02-opengl_render)
