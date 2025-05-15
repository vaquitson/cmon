cmake_minimum_required(VERSION 3.10)

project(cmon C)

enable_testing()

include_directories(${CMAKE_SOURCE_DIR}/src/lib/)
include_directories(${CMAKE_SOURCE_DIR}/src/lib/utils)
include_directories(${CMAKE_SOURCE_DIR}/src/lib/parser)

file(GLOB LIB_C_FILES ${CMAKE_SOURCE_DIR}/src/lib/*.c)
file(GLOB UTILS_C_FILES ${CMAKE_SOURCE_DIR}/src/lib/utils/*.c)
file(GLOB PARSER_C_FILES ${CMAKE_SOURCE_DIR}/src/lib/parser/*.c)

if(BUILD_TESTING)
    add_executable(string_test ./tests/src/cmon_string/test.c ${LIB_C_FILES} ${UTILS_C_FILES} ${PARSER_C_FILES}) 
    add_test(NAME test_cmon_string COMMAND ./tests/bin/cmon_string/test)
endif()

set(DCMAKE_EXPORT_COMPILE_COMMANDS 1)
add_executable(cmon ./src/main.c ${LIB_C_FILES} ${PARSER_C_FILES} ${UTILS_C_FILES}) 
