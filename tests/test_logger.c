#include <stdio.h>
#include <string.h>
#include "logger.h"

int test_log_init() {
  char buff[100];
  char *ptr = buff;
  char test_message[] = "test message from test logger.c\n";

  Logger *log_p = log_init();
  printf("%s\n", log_p->file_path);
  fputs(test_message, log_p->file_p);
  fflush(log_p->file_p);

  FILE *log_file = fopen(log_p->file_path, "r");
  while(fgets(buff, 100, log_file) != NULL) {}

  if (strncmp(buff, test_message, 40) != 0) {  
    printf("test -test_log_init-: faild -> the last log is not the correct one %s get %s\n", test_message, buff);
    return 1;
  }

  Logger *log_p2 = log_init();
  Logger *log_p3 = log_init();

  if (log_p != log_p2 && log_p3 != log_p2) {
    printf("test -test_log_init-: faild -> the singleton feature is not working\n");
    return 1;
  }

  char str[] =  "string agregada";
  log_write(log_p2, LOG_INFO, "hola %s %d\n", str, 90);

  printf("test -test_log_init-: success\n");
  return 1;
}

int main(){
  if (test_log_init() != 0){
    return 1;
  }
  test_log_init();
  return 0;
}
