#pragma once

#include <stdio.h>

#include "tokenizer.h"
#include "cmon_string_array.h"

#define MAX_N_OF_INGNORE_FILES 10
#define MAX_N_OF_INGNORE_EXT_NAMES 10
#define MAX_N_OF_INGNORE_DIRS 10

#define MAX_LEXEM_LEN 100

#define DEFAULT_CONFIG \
  "WATCH_FILE_EXT_NAMES: {\n"\
  "  \"html\",\n"\
  "  \"css\"\n"\
  "}\n"\
  "\n"\
  "IGNORE_FILES: {\n"\
  "  \".gitignore\",\n"\
  "  \"c000mand1.go\"\n"\
  "}\n"\
  "\n"\
  "IGNORE_DIRS: {\n"\
  "  \".git\",\n"\
  "  \"build\",\n"\
  "  \"node_modules\",\n"\
  "  \"si/super/com\"\n"\
  "}"

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
  CmonString *exe;
  char **argv;
  int argc;
} CmonConfig;

enum ConfigEntry {
  WATCH_FILE_EXT_NAMES,
  IGNORE_FILES,
  IGNORE_DIRS
};


// get a char buffer containig the cwd.
// the caller is responsable for the memory
CmonString *_get_cwd();

// parse the argv parameters to the CmonCommand struct
struct CmonCommand *cmon_parse_argv(int argc, char **argv);

FILE *config_open_file(const CmonString *Strpath);

int cmon_parse_config(FILE *configFile, CmonConfig *config);

/*
 * allocate a new CmonConfig structure. The caller is risponsible
 * for release the memory
*/ 
CmonConfig *config_new();

/* print the confgi to std */
void config_print(CmonConfig *config);

/*
 * convert the struct TokenArr into the structures in the 
 * CmonConfig structure
*/ 
int config_parser(struct TokenArr *token_arr, CmonConfig *conf);

/* initialize the config, which mens fill the config estructure */
void config_init(CmonConfig *conf, int argc, char **argv);

/* free the config structure */
int config_free(CmonConfig *conf);


