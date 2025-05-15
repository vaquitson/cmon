#pragma once

#include <stdio.h>
#include "cmon_string_array.h"

#define MAX_N_OF_INGNORE_FILES 10
#define MAX_N_OF_INGNORE_EXT_NAMES 10
#define MAX_N_OF_INGNORE_DIRS 10

#define MAX_LEXEM_LEN 100

#define DEFAULT_CONFIG "WATCH_FILE_EXT_NAMES: {\n\
  html,\n\
  css,\n\
  }\n\
  \n\
  IGNORE_FILES: {\n\
    .gitignore,\n\
  }\n\
  \n\
  IGNORE_DIRS: {\n\
    .git,\n\
    build,\n\
    node_modules,\n\
  }"


// the first argument of argv should be the processName
struct CmonCommand {
  char *exe;
  char *processName;  
  int argc;
  char *argv[10];
};


// [FIXME] this probably should be in a more memory eficient way 
// and also more flexible
typedef struct {
  CmonStringArray ignoreFiles;
  CmonStringArray ignoreDirs;
  CmonStringArray watchExtNames;
  
  CmonString *cwd;
} CmonConfig;

enum ConfigEntry {
  WATCH_FILE_EXT_NAMES,
  IGNORE_FILES,
  IGNORE_DIRS
};


// get a char buffer containig the cwd.
// the caller is responsable for the memory
CmonString *cmon_get_cwd();

// parse the argv parameters to the CmonCommand struct
struct CmonCommand *cmon_parse_argv(int argc, char **argv);

FILE *cmon_open_config_file(const CmonString *Strpath);

int cmon_parse_config(FILE *configFile, CmonConfig *config);

CmonConfig *cmon_config_new();

void cmon_print_config(CmonConfig *config);
