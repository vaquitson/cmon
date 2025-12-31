file(GLOB LOGGER_C_FILES ${CMAKE_SOURCE_DIR}/src/lib/logger/*.c)
file(GLOB UTILS_C_FILES ${CMAKE_SOURCE_DIR}/src/lib/utils/*.c)

include_directories(${CMAKE_SOURCE_DIR}/src/lib/logger)
include_directories(${CMAKE_SOURCE_DIR}/src/lib/utils)

add_executable(test_logger)
target_sources(test_logger
  PRIVATE
  ./test_logger.c
  ./test_macros.h
  ${LOGGER_C_FILES})

add_executable(test_cmon_path)
target_sources(test_cmon_path
  PRIVATE
  ./test_cmon_path.c
  ./test_macros.h
  ${LOGGER_C_FILES}
  ${UTILS_C_FILES})


