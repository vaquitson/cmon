#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "cmon_errors.h"
#include "cmon_print.h"
#include "config.h"

struct cmon_config {
  size_t pathMaxsize;
};


#define DEFAULT_CONFIG "{\
  \"WATCH_FILE_EXT_NAMES\": [\"html\", \"css\"],\
  \"IGNORE_FILES\": [\".gitignore\"],\
  \"IGNORE_DIRS\": [\".git\", \"build\", \"node_modules\"]\
}"


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


// create the struct CmonCommand with the infromation comming from argv
struct CmonCommand *cmon_parse_argv(int argc, char **argv){
  struct CmonCommand *command = malloc(sizeof(struct CmonCommand)); 
  int commandArgvIndex = 1;
  int argvIndex = 2;

  if (argc < 2){
    cmon_print_error(true, "cmon_parse_argv", "there is no exe in the arguments");
    exit(1);
  }

  command = malloc(sizeof(struct CmonCommand)); 

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



// this functions open the default configFile
// if it not exist it create one with a default config
FILE *cmon_open_config_file(const char *path){
  FILE *configFile;
  int err;
  // the file does not exist
  if (access(path, R_OK) == -1){
    cmon_print_msg_to_user("[WARNING] the config file does not exist");
    err = errno;
    if (err == ENOENT){
      printf("path: %s\n",path );
      configFile = fopen(path, "w+");
      if (!configFile){
        err = errno;
        cmon_print_errno_error(true, "cmon_open_config_file", err, "something went wrong");
      }
      fprintf(configFile, DEFAULT_CONFIG);
      fflush(configFile);
      return configFile;
    } else {
      cmon_print_errno_error(true, "cmon_open_config_file", err, "something went wrong");
      exit(1);
    }
  } else { 
    configFile = fopen(path, "r");
    if (!configFile){
      err = errno;
      cmon_print_errno_error(true, "cmon_open_config_file", err, "something went wrong");
      exit(1);
    }
    return configFile;
  }
}



