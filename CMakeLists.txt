cmake_minimum_required(VERSION 3.10)

project(cmon C)

enable_testing()

include_directories(${CMAKE_SOURCE_DIR}/src/lib/)
include_directories(${CMAKE_SOURCE_DIR}/src/lib/utils)
include_directories(${CMAKE_SOURCE_DIR}/src/lib/parser)
include_directories(${CMAKE_SOURCE_DIR}/src/lib/config)
include_directories(${CMAKE_SOURCE_DIR}/src/lib/ps_tree)
include_directories(${CMAKE_SOURCE_DIR}/src/lib/process)
include_directories(${CMAKE_SOURCE_DIR}/src/lib/inotify)

file(GLOB LIB_C_FILES ${CMAKE_SOURCE_DIR}/src/lib/*.c)
file(GLOB UTILS_C_FILES ${CMAKE_SOURCE_DIR}/src/lib/utils/*.c)
file(GLOB PARSER_C_FILES ${CMAKE_SOURCE_DIR}/src/lib/parser/*.c)
file(GLOB CONFIG_C_FILES ${CMAKE_SOURCE_DIR}/src/lib/config/*.c)
file(GLOB PS_TREE_C_FILES ${CMAKE_SOURCE_DIR}/src/lib/ps_tree/*.c)
file(GLOB PROCESS_C_FILES ${CMAKE_SOURCE_DIR}/src/lib/process/*.c)
file(GLOB INOTIFY_C_FILES ${CMAKE_SOURCE_DIR}/src/lib/inotify/*.c)


# tokenizer test
add_executable(T_tokenizer ./tests/src/T_tokenizer.c 
                           ${LIB_C_FILES} 
                           ${UTILS_C_FILES} 
                           ${PARSER_C_FILES})
set_target_properties(T_tokenizer PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests
)
add_test(NAME T_tokenizer COMMAND ./build/tests/T_tokenizer)

# config parser test
add_executable(T_config_parser ./tests/src/T_config_parser.c 
                               ${LIB_C_FILES} 
                               ${UTILS_C_FILES} 
                               ${PARSER_C_FILES} 
                               ${CONFIG_C_FILES})
set_target_properties(T_config_parser PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests
)
add_test(NAME T_config_parser COMMAND ./build/tests/T_config_parser)

# ps tree test 
add_executable(T_ps_tree ./tests/src/T_ps_tree.c 
                         ${LIB_C_FILES} 
                         ${UTILS_C_FILES} 
                         ${PS_TREE_C_FILES}) 
set_target_properties(T_ps_tree PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests
)
add_test(NAME T_ps_tree COMMAND ./build/tests/T_ps_tree)


set(DCMAKE_EXPORT_COMPILE_COMMANDS 1)
add_executable(cmon ./src/main.c ${LIB_C_FILES} ${UTILS_C_FILES} ${PARSER_C_FILES} ${CONFIG_C_FILES} ${PS_TREE_C_FILES} ${PROCESS_C_FILES} ${INOTIFY_C_FILES}) 
