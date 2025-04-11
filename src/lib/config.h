#pragma once

#include <stdio.h>

#define MAX_NUMBER_OF_WATCH_EXT_NAMES 10

struct cmon_fonfig {
  char *TARGET_EXT_NAME[MAX_NUMBER_OF_WATCH_EXT_NAMES];

};

FILE *cmon_open_config_file();

// get a char buffer containig the cwd.
// the caller is responsable for the memory
char *cmon_get_cwd();
