file(GLOB LOGGER_C_FILES ${CMAKE_SOURCE_DIR}/src/lib/logger/*.c)
file(GLOB UTILS_C_FILES ${CMAKE_SOURCE_DIR}/src/lib/utils/*.c)
file(GLOB SSE_C_FILES ${CMAKE_SOURCE_DIR}/src/sse/*.c)

include_directories(${CMAKE_SOURCE_DIR}/src/lib/logger)
include_directories(${CMAKE_SOURCE_DIR}/src/lib/utils)
include_directories(${CMAKE_SOURCE_DIR}/src/sse)

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


add_executable(test_cmon_sse)
target_sources(test_cmon_sse
  PRIVATE
  ./test_cmon_sse.c
  ./test_macros.h
  ${LOGGER_C_FILES}
  ${SSE_C_FILES})

