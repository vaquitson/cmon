#pragma once
#include <stdio.h>
#include <unistd.h>

enum {
  LOG_DEBUG,
  LOG_INFO,
  LOG_WARNING,
  LOG_ERROR
};

typedef struct {
  FILE *file_p;
  pid_t pid;
  char *file_path;
} Logger;


Logger *log_init();
void log_write(int priority, char *meesage, ...);

