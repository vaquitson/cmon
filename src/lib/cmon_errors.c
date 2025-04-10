#include <stdio.h>
#include "cmon_errors.h"
#include <stdbool.h>


int cmonAlowLog = true;
static int defoultPirntFd = 1;

int cmon_log(const char *status, const char *functionName, const char *msg){  
  static FILE *cmonLogF = NULL;
  if (cmonAlowLog){
    if (!cmonLogF){
      cmonLogF = fopen("./../../temp/log", "w");
      if (!cmonLogF){
        return -1;
      }
      fprintf(cmonLogF, "[%s]: from fucntion %s: %s\n", status, functionName, msg);
      return 0;
    }

  }
  return -2;
}

int cmon_print_error(bool log, const char *functionName, const char *msg){
  dprintf(defoultPirntFd, "Cmon ERROR on function %s: %s\n", functionName, msg);
  cmon_log("ERROR", functionName, msg);
  return 0;
}

