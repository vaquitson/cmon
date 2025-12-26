cmake_minimum_required(VERSION 3.10)
project(cmon)

enable_testing()
add_subdirectory(tests)

include_directories(${CMAKE_SOURCE_DIR}/src/lib/utils)
include_directories(${CMAKE_SOURCE_DIR}/src/lib/parser)
include_directories(${CMAKE_SOURCE_DIR}/src/lib/config)
include_directories(${CMAKE_SOURCE_DIR}/src/lib/ps_tree)
include_directories(${CMAKE_SOURCE_DIR}/src/lib/process)
include_directories(${CMAKE_SOURCE_DIR}/src/lib/inotify)
include_directories(${CMAKE_SOURCE_DIR}/src/lib/logger)

file(GLOB UTILS_C_FILES ${CMAKE_SOURCE_DIR}/src/lib/utils/*.c)
file(GLOB PARSER_C_FILES ${CMAKE_SOURCE_DIR}/src/lib/parser/*.c)
file(GLOB CONFIG_C_FILES ${CMAKE_SOURCE_DIR}/src/lib/config/*.c)
file(GLOB PS_TREE_C_FILES ${CMAKE_SOURCE_DIR}/src/lib/ps_tree/*.c)
file(GLOB PROCESS_C_FILES ${CMAKE_SOURCE_DIR}/src/lib/process/*.c)
file(GLOB INOTIFY_C_FILES ${CMAKE_SOURCE_DIR}/src/lib/inotify/*.c)
file(GLOB LOGGER_C_FILES ${CMAKE_SOURCE_DIR}/src/lib/logger/*.c)

add_executable(cmon) 

target_compile_options(cmon 
  PRIVATE
  -Wall -Wextra -Wpedantic
)

target_sources(cmon
  PRIVATE
  ./src/main.c
  ${LIB_C_FILES} 
  ${UTILS_C_FILES} 
  ${PARSER_C_FILES} 
  ${CONFIG_C_FILES} 
  ${PS_TREE_C_FILES} 
  ${PROCESS_C_FILES} 
  ${INOTIFY_C_FILES}
  ${LOGGER_C_FILES}
) 

set(DCMAKE_EXPORT_COMPILE_COMMANDS 1)
