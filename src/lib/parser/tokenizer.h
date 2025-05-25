#pragma once

#include <stdio.h>
#include <stdbool.h>

enum TokenTypes {
  TEXT,
  STRING,
  DOUBLE_COLON,
  CURLY_BARACKETS,
  O_CURLY_BARACKET,
  C_CURLY_BARACKET,
  PARENTHESIS,
  COLON,
  NUMBER,
};

struct Token {
  unsigned int type;
  char *value;
};

// [FIXME]: this is an array
struct TokenArr {
  struct Token tokenList[100]; 
  struct Token *nextPosition;
  int length;
};



struct TokenArr *token_arr_new();

struct TokenArr *cmon_tokenizer(FILE *configFile);

struct TokenArr *config_tokenizer(struct TokenArr *tokArr, FILE *configFile);

/* this fucntio n prints the elemnts of the token array provided */
int token_arr_print(struct TokenArr *tokArr);

/* this functions free all the memory asociated with the token array provided. */
void token_arr_free(struct TokenArr *tokArr);

