file(GLOB LOGGER_C_FILES ${CMAKE_SOURCE_DIR}/src/lib/logger/*.c)
include_directories(${CMAKE_SOURCE_DIR}/src/lib/logger)

add_executable(test_logger)

target_sources(test_logger
  PRIVATE
  ./test_logger.c
  ${LOGGER_C_FILES})


