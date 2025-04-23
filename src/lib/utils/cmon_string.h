
#pragma once

#define MAX_STRING_LEN 250

typedef struct{
  int stringLen; // without NULL char
  char string[];
} CmonString;


// this create a new allocated CmonString 
// the caller of the function is responsible for 
// freeing the memori
CmonString *cmon_string_new(const char *string);








