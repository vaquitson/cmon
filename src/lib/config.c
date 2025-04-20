#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>

#include "cmon_errors.h"
#include "cmon_print.h"
#include "config.h"



// create a token in an empty position of the token list 
// in succes return a pointer to the new allocated token in the token list
// in error return NULL
struct Token *_cmon_new_token(struct TokenList *tokenList, int type, char *string){
  struct Token *token;

  if (!string){
    return NULL;
  }

  if (string[0] == '\0'){
    return NULL;
  }
  
  if (!tokenList){
    return NULL;
  }

  token = (struct Token *)malloc(sizeof(struct Token));
  if (!token){
    return NULL;
  }

  switch (type){
    case LIST_MARK:
      tokenList->tokenList[tokenList->length].value = (char *)malloc(2);
      if (!tokenList->tokenList[tokenList->length].value){
        return NULL;
      }
      tokenList->tokenList[tokenList->length].value[0] = string[0];
      tokenList->tokenList[tokenList->length].value[1] = '\0';
      break;

    case STRING:
      tokenList->tokenList[tokenList->length].value = strdup(string);
      break;

    case COLON:
      tokenList->tokenList[tokenList->length].value = (char *)malloc(2);
      if (!tokenList->tokenList[tokenList->length].value){
        return NULL;
      }
      tokenList->tokenList[tokenList->length].value[0] = string[0];
      tokenList->tokenList[tokenList->length].value[1] = '\0';
      break;
  }
  
  tokenList->tokenList[tokenList->length].type = type; 
  tokenList->length++;
  
  return &tokenList->tokenList[tokenList->length-1]; 
}


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

struct CmonConfig *cmon_init_config(){
  struct CmonConfig *config = (struct CmonConfig *)malloc(sizeof(struct CmonConfig));
  if (!config){
    cmon_print_error(true, "cmon_init_config", "could not allocate memory for the struct CmonConfig");
    exit(1);
  }
  config->ignoreDirsLen = 0;
  config->ignoreFilesLen = 0;
  config->watchExtNamesLen = 0;

  return config;
}


int _cmon_add_string_to_config_entry(struct CmonConfig *config, int configEntry, char *str){
  if (!config){
    cmon_print_error(true, "_cmon_add_string_to_fonfig_entry", "the config was not provided");
    exit(1);
  }

  int entryLen;
  char *entry;
  
  switch (configEntry){
    case WATCH_FILE_EXT_NAMES:
      entry = config->watchExtNames[entryLen];

      break;
    case IGNORE_FILES:
      break;
    case IGNORE_DIRS:
      break;
  }
  return 0; 
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


// this function takes the config file and make a malloc allocated struct tokenList
// the caller is responsible to free the memory
struct TokenList *_cmon_tokenizer(FILE *configFile){
  int buffSize = 2048;
  char buff[buffSize];
  char lexem[MAX_LEXEM_LEN];
  char *buffPtr;
  char *lexemPtr;
  
  buffPtr = buff;
  lexemPtr = lexem;

  struct TokenList *tokenList = (struct TokenList *)malloc(sizeof(struct TokenList));
  tokenList->length = 0;

  while(fgets(buff, buffSize, configFile)){
    buffPtr = buff;
    while(*buffPtr != '\0'){
      
      if ((*buffPtr >= 'a' && *buffPtr <= 'z') || (*buffPtr >= 'A' && *buffPtr <= 'Z') || (*buffPtr == '_') || (*buffPtr) == '.'){
        *lexemPtr = *buffPtr;
        lexemPtr++;
      } 

      else if (*buffPtr == '{' || *buffPtr == '}'){
        if (lexemPtr > lexem){
          _cmon_new_token(tokenList, STRING, lexem);
          lexemPtr = lexem;
        }
        _cmon_new_token(tokenList, LIST_MARK, buffPtr);
      }


      else if (*buffPtr == ' '){
        if (lexemPtr > lexem){
          *lexemPtr = '\0';
          _cmon_new_token(tokenList, STRING, lexem);
          lexemPtr = lexem;
        }
      }

      else if (*buffPtr == ','){
        if (lexemPtr > lexem){
          *lexemPtr = '\0';
          _cmon_new_token(tokenList, STRING, lexem);
          lexemPtr = lexem;
        }
      }

      else if (*buffPtr == ':'){
        if (lexemPtr > lexem){
          _cmon_new_token(tokenList, STRING, lexem);
          lexemPtr = lexem;
        }

        *lexemPtr = '\0';
        _cmon_new_token(tokenList, COLON, buffPtr);
      }
      buffPtr++;
    }
  } 
  return tokenList;
}

// parse the config file into the config struct
int cmon_parse_config(FILE *configFile, struct CmonConfig *config){
  struct Token *curTok; 
  struct Token *nextTok; 
  struct TokenList *tokenList = _cmon_tokenizer(configFile);
  char **configTarget = NULL; 

  for (int i = 0; i < tokenList->length; i++){
    curTok = &tokenList->tokenList[i]; 
    switch (curTok->type) {
      case STRING:
        if (i + 1 < tokenList->length){
          if (tokenList->tokenList[i+1].type == COLON){
            if (strcmp(curTok->value, "WATCH_FILE_EXT_NAMES") == 0){
                             
            }
          }
        }
    }
  }

  return 0;
}
