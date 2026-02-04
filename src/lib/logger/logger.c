/* this file contains the logic and the API to create log messages
*/

#include <time.h>
#include <logger.h>
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <stdarg.h>

#include "logger.h"

#define MAX_PATH_SIZE 2048

// intialize the log stream and return singleton to the Logger structure
// if the function was previously call, return the previously initialized
// Logger pointer 
// The if the logger faild, the program will exit
Logger *log_init(void){
  static Logger *logger = NULL;
  if (logger != NULL){
    return logger;
  }

  logger = (Logger *)malloc(sizeof(Logger));
  if (logger == NULL){
    printf("from log_init: could not allocate memory for the Logger");
    exit(1);
  }

  char *log_path = (char *)malloc(MAX_PATH_SIZE);
  if (log_path == NULL){
    printf("from log_init: could not allocate memory for the log path");
    exit(2);
  }

  char *path_sufix;
  char *env_path;
  if ((env_path = getenv("XDG_STATE_HOME")) == NULL){
    if ((env_path = getenv("HOME")) == NULL) {
      printf("Error: the HOME and the XDG_STATE_HOME are not set");
      exit(errno);
    }
    path_sufix = "/.local/state/cmon.log";
  } else {
    path_sufix = "/cmon.log";
  }

  if (snprintf(log_path, MAX_PATH_SIZE, "%s%s", env_path, path_sufix) > MAX_PATH_SIZE) {
    printf("Error: the path for the log file is to large: %s\n", log_path);
    exit(errno);
  }
  logger->file_path = log_path;

  if ((logger->file_p = fopen(log_path, "a")) == NULL){
    printf("Error: the path %s could not be open: %s\n", log_path, strerror(errno));
    exit(errno);
  }

  logger->pid = getpid();

  return logger; 
}


// write a log
void log_write(int priority, char *meesage, ...){
  static Logger *logger = NULL;
  if (logger == NULL) {
    logger = log_init();
  }

  static char *priority_str;

  time_t t = time(NULL);
  struct tm tm = *localtime(&t);

  va_list ap;
  va_start(ap, meesage);

  switch (priority) {
    case LOG_DEBUG:
      priority_str = "DEBUG";
      break;
    case LOG_INFO:
      priority_str = "INFO";
      break;
    case LOG_WARNING:
      priority_str = "WARNING";
      break;
    case LOG_ERROR:
      priority_str = "ERROR";
      break;
    default:
      priority_str = "UNDEFINED";
  }
  
  fprintf(logger->file_p, "%d | %d-%d-%d | %d:%d:%d | %s | ", 
      logger->pid, tm.tm_year + 1900, tm.tm_mon, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, priority_str);
  vfprintf(logger->file_p, meesage, ap);
  fputc('\n', logger->file_p);
  fflush(logger->file_p);
  va_end(ap);
}
