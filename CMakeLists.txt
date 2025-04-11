cmake_minimum_required(VERSION 3.10)

project(cmon C)

include_directories(${CMAKE_SOURCE_DIR}/src/lib/)

file(GLOB LIB_C_FILES ${CMAKE_SOURCE_DIR}/src/lib/*.c)

set(DCMAKE_EXPORT_COMPILE_COMMANDS 1)
add_executable(cmon ./src/main.c ${LIB_C_FILES}) 
