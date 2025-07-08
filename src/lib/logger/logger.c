#include <logger.h>
#include <stdio.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include "logger.h"
#define LOG_DEFOULT_RELATIVE_PATH "./../../../temp/app.log"

static FILE *log_fd;
static char LOG_BUFF[2048];
static int log_max_byte_size = 1024*100;

void log_init(){
  FILE *log_fd;
  off_t log_size;
  struct stat info;
   
  if (!stat(LOG_DEFOULT_RELATIVE_PATH, &info)){
    printf("panic: could not find info for the log file: %s\n", strerror(errno)); 
    exit(1);
  };

  if (info.st_size > (log_max_byte_size)){
    printf("hola\n");
    log_fd = fopen(LOG_DEFOULT_RELATIVE_PATH, "r");
  } else {
    log_fd = fopen(LOG_DEFOULT_RELATIVE_PATH, "a");
  }
  if (!log_fd){
    printf("panic: could not open the log file: %s\n", strerror(errno)); 
    exit(2);
  }
}

void log_log(int level, int module, char *msg){
  fprintf(log_fd, "hello");  
}
