#pragma once

#define MAX_STRING_LEN 250
#define MAX_AMOUNT_OF_STRINGS 50

#include <stdbool.h>

// this strings are inmutable
typedef struct{
  unsigned int len; // without NULL char
  char string[];
} CmonString;

// this macro gets the char arra of the cmon string
#define cmon_str_get(str) (str)->string

// this macro gets the len of a cmon string
#define cmon_str_len(str) (str)->len

// this create a new allocated CmonString 
// the caller of the function is responsible for 
// freeing the memori
CmonString *cmon_str_new(char *charArr);

// create a new heap allocated CmonString from the CmonString provided in 
// the parameters 
// the caller of the function is risposible for the memory
CmonString *cmon_str_new_from_str(CmonString *string, ...);

// create a new heap allocated CmonString from char arrays
// the caller of the function is risponsible for the new allocated
// CmonString 
CmonString *cmon_str_new_from_char_arrs(char *charArr, ...);

// this functuon compare two CmonStrings, return true if there are
// equal and false if there are diferent
bool cmon_str_cmp(CmonString *a, CmonString *b);



