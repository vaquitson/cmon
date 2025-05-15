#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>

#include "cmon_errors.h"
#include "cmon_print.h"
#include "cmon_config.h"
#include "cmon_parser.h"


// print to std the config structure.
void cmon_print_config(CmonConfig *config){
  int i = 0;
  printf("cwd: %s\n\n", config->cwd->string);

  printf("Ignore Dirs\n");
  for (i = 0; i < config->ignoreDirs.len; i++){
    printf("\t%s\n", config->ignoreDirs.arr[i]->string); 
  }

  printf("Ignore Files\n");
  for (i = 0; i < config->ignoreFiles.len; i++){
    printf("\t%s\n", config->ignoreFiles.arr[i]->string); 
  }

  printf("Watch ext\n");
  for (i = 0; i < config->watchExtNames.len; i++){
    printf("\t%s\n", config->watchExtNames.arr[i]->string); 
  }
}


// this funtion returns the cwd as a CmonString, the format is 
// /home/.../.../ endig with a /
// if the function fails, return NULL;
// the caller of the function is risponsible for the memroy
CmonString *cmon_get_cwd(){
  long pathMax;
  size_t size;
  char *charCwd;
  char *ptr;
  CmonString *cwd;

  // check the max path length 
  pathMax = pathconf(".", _PC_PATH_MAX); 
  if (pathMax == -1){
    size = 4096; // 4kb
  } else if (pathMax > 10240){
    size = 10240; // 10kb
  } else {
    size = pathMax; 
  }
  charCwd = (char *)malloc(pathMax); 

  if (!charCwd){
    cmon_print_error(true, "cmon_get_cwd", "could not allocate memmory for the cwd");
    return NULL;
  }
  ptr = getcwd(charCwd, size);

  if (!ptr){
    cmon_print_error(true, "cmon_get_cwd", "could not get current worrking directori");
    return NULL;
  }
  
  cwd = cmon_str_new_from_char_arrs(charCwd, "/", NULL);
  free(charCwd);
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


// create a heap allocated (malloc) CmonConfig 
// the caller of the function is responsible for free the memory
CmonConfig *cmon_config_new(){
  CmonConfig *config = (CmonConfig *)malloc(sizeof(CmonConfig));
  if (!config){
    cmon_print_error(true, "cmon_init_config", "could not allocate memory for the struct CmonConfig");
    exit(1);
  }

  cmon_str_arr_init(&config->ignoreDirs);
  cmon_str_arr_init(&config->ignoreFiles);
  cmon_str_arr_init(&config->watchExtNames);

  return config;
}


// this functions open the default configFile
// if it not exist it create one with a default config
FILE *cmon_open_config_file(const CmonString *StrPath){
  const char *path = StrPath->string;

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


// get the token at index i from the token list 
// if the index is larger than the token list return null
struct Token *_cmon_get_token(struct TokenList *tokenList, int i){
  if (i < 0){
    cmon_print_error(true, "_cmon_get_token", "the index is not valid");
    return NULL;
  }

  if (i >= tokenList->length){
    return NULL; 
  }

  return &tokenList->tokenList[i];
}


// parse the config file into the config struct
int cmon_parse_config(FILE *configFile, CmonConfig *conf){
  struct Token *curTok; 
  struct Token *nextTok; 
  struct TokenList *tokenList = cmon_tokenizer(configFile);

  CmonStringArray *configTarget = NULL; 
  CmonString *newStr;
  int confId;
  bool inBlock = false;

  for (int i = 0; i < tokenList->length; i++){
    curTok = _cmon_get_token(tokenList, i);

    switch (curTok->type) {
      case STRING:
        if (configTarget != NULL && inBlock == true){
          if (confId == IGNORE_DIRS || confId == IGNORE_FILES){
            newStr = cmon_str_new_from_char_arrs(cmon_str_get(conf->cwd), curTok->value, NULL);; 
            cmon_str_arr_add_from_str(configTarget, newStr);
          } else {
            cmon_str_arr_add_new_char_arr(configTarget, curTok->value);
          }
        }
        break;

      case LIST_MARK:
        if (curTok->value[0] == '{'){
          if (inBlock == false){
            inBlock = true;
          } else {
            cmon_print_error(true, "cmon_parse_config", "unexpected { in the config file");
            exit(1);
          }
        }
        else if (curTok->value[0] == '}'){
          if (inBlock == true){
            inBlock = false;
          }
        } else {
          cmon_print_error(true, "cmon_parse_config", "unexpected } in the config file");
          exit(1);
        }
        break;

      case CONFIG_ENTRY:
        if (strcmp(curTok->value, "WATCH_FILE_EXT_NAMES") == 0){          
          configTarget = &conf->watchExtNames;
          confId = WATCH_FILE_EXT_NAMES;
        }

        else if (strcmp(curTok->value, "IGNORE_FILES") == 0){
          configTarget = &conf->ignoreFiles;
          confId = IGNORE_FILES;
        }

        else if (strcmp(curTok->value, "IGNORE_DIRS") == 0){
          configTarget = &conf->ignoreDirs;
          confId = IGNORE_DIRS;

        } else {
          cmon_print_error(true, "cmon_parse_config", "the config entry dont exist");
          exit(1);
        }
        break;
    }
  }
  return 0;
}
