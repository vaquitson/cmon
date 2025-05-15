#pragma once

#include <stdio.h>

enum TokenTypes {
  CONFIG_ENTRY,
  STRING,
  LIST_MARK,
  COLON,
  NUMBER,
};

struct Token {
  unsigned int type;
  char *value;
};


struct TokenList {
  struct Token tokenList[100]; 
  int length;
};


struct TokenList *cmon_tokenizer(FILE *configFile);


