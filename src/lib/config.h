#pragma once

#include <stdio.h>

#define MAX_NUMBER_OF_WATCH_EXT_NAMES 10

// the first argument of argv should be the processName
struct CmonCommand {
  char *exe;
  char *processName;  
  int argc;
  char *argv[10];
};


struct cmonConfig {
  char *TARGET_EXT_NAME[MAX_NUMBER_OF_WATCH_EXT_NAMES];
};


// get a char buffer containig the cwd.
// the caller is responsable for the memory
char *cmon_get_cwd();

// parse the argv parameters to the CmonCommand struct
struct CmonCommand *cmon_parse_argv(int argc, char **argv);


FILE *cmon_open_config_file(const char *path);

