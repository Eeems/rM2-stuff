cmake_minimum_required(VERSION 3.10)

project(rm2-swtcon VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wno-psabi")

add_compile_options(-fdiagnostics-color=always)

add_subdirectory(libs)
add_subdirectory(tools)
add_subdirectory(apps)
