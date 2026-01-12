cmake_minimum_required(VERSION 3.11)
project(cmon C)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)

FetchContent_Declare(llhttp
  URL "https://github.com/nodejs/llhttp/archive/refs/tags/release/v8.1.0.tar.gz"
)

FetchContent_MakeAvailable(llhttp)

file(GLOB UTILS_C_FILES   ${CMAKE_SOURCE_DIR}/src/lib/utils/*.c)
file(GLOB PARSER_C_FILES  ${CMAKE_SOURCE_DIR}/src/lib/parser/*.c)
file(GLOB CONFIG_C_FILES  ${CMAKE_SOURCE_DIR}/src/lib/config/*.c)
file(GLOB PS_TREE_C_FILES ${CMAKE_SOURCE_DIR}/src/lib/ps_tree/*.c)
file(GLOB PROCESS_C_FILES ${CMAKE_SOURCE_DIR}/src/lib/process/*.c)
file(GLOB INOTIFY_C_FILES ${CMAKE_SOURCE_DIR}/src/lib/inotify/*.c)
file(GLOB LOGGER_C_FILES  ${CMAKE_SOURCE_DIR}/src/lib/logger/*.c)
file(GLOB SSE_C_FILES     ${CMAKE_SOURCE_DIR}/src/cmon_proxy/*.c)

add_executable(cmon
  ./src/main.c
  ${UTILS_C_FILES}
  ${PARSER_C_FILES}
  ${CONFIG_C_FILES}
  ${PS_TREE_C_FILES}
  ${PROCESS_C_FILES}
  ${INOTIFY_C_FILES}
  ${LOGGER_C_FILES}
  ${SSE_C_FILES}
)

target_compile_options(cmon PRIVATE -Wall -Wextra -Wpedantic)

target_include_directories(cmon PRIVATE
  ${llhttp_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/src/lib/utils
  ${CMAKE_SOURCE_DIR}/src/lib/parser
  ${CMAKE_SOURCE_DIR}/src/lib/config
  ${CMAKE_SOURCE_DIR}/src/lib/ps_tree
  ${CMAKE_SOURCE_DIR}/src/lib/process
  ${CMAKE_SOURCE_DIR}/src/lib/inotify
  ${CMAKE_SOURCE_DIR}/src/lib/logger
  ${CMAKE_SOURCE_DIR}/src/cmon_proxy
)

enable_testing()
add_subdirectory(tests)


