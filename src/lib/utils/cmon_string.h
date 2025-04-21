
#pragma once

#define MAX_STRING_LEN 250
#define INITIAL_STRING_SIZE 10 


typedef struct{
  unsigned int stringLen; // without NULL char
  char string[];
} CmonString;


CmonString *cmon_string_new(char *string);








