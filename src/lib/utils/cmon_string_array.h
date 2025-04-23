#pragma once

#include "cmon_string.h"

#define INITIAL_ARRAY_SIZE 50

typedef struct  {
  unsigned int len; 
  CmonString *arr[INITIAL_ARRAY_SIZE];
} CmonStringArray;


// add a new allocated CmonString (from str) to the arr
// if error or the array is full, NULL is return
CmonString *cmon_str_arr_add_new_str(CmonStringArray *arr, const char* str);

// create a new string array
CmonStringArray *cmon_str_arr_new();

// init a CmonStringArray from the arr pointer
// if err null is retunr
CmonStringArray *cmon_str_arr_init(CmonStringArray *arr);

// get the CmonString at index position in the arr 
// if something went wrong NULL is retunr
CmonString *cmon_str_arr_get_str(CmonStringArray *arr, int index);


// get the len of the CmonStringArray
int cmon_str_arr_get_len(CmonStringArray *arr);

