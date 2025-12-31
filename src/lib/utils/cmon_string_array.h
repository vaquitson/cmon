#pragma once

#include "cmon_string.h"

#define INITIAL_ARRAY_SIZE 50

typedef struct  {
  size_t len; 
  CmonString *arr[INITIAL_ARRAY_SIZE];
} CmonStringArray;


#define cmon_str_arr_len(strArray) (strArray)->len

// add a new allocated CmonString (from str) to the arr
// if error or the array is full, NULL is return
CmonString *cmon_str_arr_add_new_char_arr(CmonStringArray *arr, char* str);

// add the provided cmon string to the CmonStringArray. In suscces return
// a pointer to the provided string, if fail return NULL
CmonString *cmon_str_arr_add_from_str(CmonStringArray *arr, CmonString *str);

// create a new string array
CmonStringArray *cmon_str_arr_new();

/* 
 * init a CmonStringArray from the arr pointer
 * if err null is retunr
 * */ 
CmonStringArray *cmon_str_arr_init(CmonStringArray *arr);

// get the CmonString at index position in the arr 
// if something went wrong NULL is retunr
CmonString *cmon_str_arr_get_str(CmonStringArray *arr, size_t index);

/* his function finds if a CmonString exists on the CmonStringArray.
   if the str exists retrun a pointer to the string find in the CmonStringArray.
   else return NULL
*/
CmonString *cmon_str_arr_find(CmonStringArray *arr, CmonString *str);

/* free the internal strings. The CmonStringArray still need to be free */
int cmon_str_arr_free(CmonStringArray *arr);
