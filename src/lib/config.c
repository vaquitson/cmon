#include <unistd.h>
#include <stdlib.h>

#include "cmon_errors.h"
#include "config.h"

struct cmon_config {
  size_t pathMaxsize;
};

char *cmon_get_cwd(){
  long pathMax;
  size_t size;
  char *cwd;
  char *ptr;

  // check the max path length 
  pathMax = pathconf(".", _PC_PATH_MAX); 
  if (pathMax == -1){
    size = 4096; // 4kb
  } else if (pathMax > 10240){
    size = 10240; // 10kb
  } else {
    size = pathMax; 
  }

  cwd = (char *)malloc(pathMax); 
  if (!cwd){
    cmon_print_error(true, "cmon_get_cwd", "could not allocate memmory for the cwd");
    exit(1);
  }
  ptr = getcwd(cwd, size);
  if (!ptr){
    cmon_print_error(true, "cmon_get_cwd", "could not get current worrking directori");
    exit(2);
  }
  return cwd;
}

FILE *cmon_open_config_file(){
  FILE *configFile;
  configFile = fopen("./../.config.cmon", "r");
  if (!configFile){
    printf("[WARNING] the .config.cmon file is not provided\n");
    return NULL;
  }
  return configFile;
}



