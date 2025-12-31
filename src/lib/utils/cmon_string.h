#pragma once

#define MAX_STRING_LEN 250
#define MAX_AMOUNT_OF_STRINGS 50

#include <stdlib.h>
#include <stdbool.h>

// this strings are inmutable
typedef struct{
  size_t len; // without NULL char
  char *string;
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

/* 
   This function takes a list of char arrays, ending with a NULL argument to indicate 
   the end of the list. It creates a new heap-allocated CmonString by combining the given arrays.
   The caller is responsible for freeing the newly allocated CmonString.
*/
CmonString *cmon_str_new_from_char_arrs(char *charArr, ...);

// this functuon compare two CmonStrings, return true if there are
// equal and false if there are diferent
bool cmon_str_cmp(CmonString *a, CmonString *b);

/* creates a new heap allocated copy of the str provided
 * if fail NULL is returnd
*/
CmonString *cmon_str_copy(CmonString *str);
