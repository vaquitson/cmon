#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "cmon_errors.h"
#include "config.h"

struct cmon_config {
  size_t pathMaxsize;
};


// functions that returns an array with
// the cwd
// the caller is responsible for freeingit
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


struct CmonCommand *cmon_parse_argv(int argc, char **argv){
  if (argc < 2){
    cmon_print_error(true, "cmon_parse_argv", "there is no exe in the arguments");
    exit(1);
  }

  struct CmonCommand *command = malloc(sizeof(struct CmonCommand)); 
  int commandArgvIndex = 1;
  int argvIndex = 2;

  if (!command){
    cmon_print_error(true, "cmon_parse_argv", "memory could not be allocated for CmonCommand");
    exit(1);
  }

  command->exe = strdup(argv[1]);
  command->processName = strdup("Juan Pablo Segundo");
  command->argv[0] = command->processName;

  if (argc >= 3){
    for (; argvIndex < argc; argvIndex++, commandArgvIndex++){
      command->argv[commandArgvIndex] = argv[argvIndex];
    }
  }

  command->argv[commandArgvIndex] = NULL;
  return command;
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



