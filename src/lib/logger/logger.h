#pragma once
#include <stdio.h>
#include <unistd.h>

#define MAX_LOG_MESSAGE_LENGTH 200

enum {
  LOG_DEBUG,
  LOG_INFO,
  LOG_WARNING,
  LOG_ERROR
};

struct Logger {
  FILE *file_p;
  pid_t pid;
  char *file_path;
} typedef Logger;


Logger *log_init();
void log_write(Logger *logger, int priority, char *meesage, ...);
