#include <stdlib.h>

#include "cmon_string.h"
#include "cmon_string_array.h"
#include "cmon_errors.h"


CmonStringArray *cmon_str_arr_new(){
  CmonStringArray *newArray = (CmonStringArray *)malloc(sizeof(CmonStringArray));
  newArray->len = 0;
  return newArray;
}

CmonStringArray *cmon_str_arr_init(CmonStringArray *arr){
  if (!arr){
    return NULL;
  }
  arr->len = 0;
  return arr;
}


CmonString *cmon_str_arr_get_str(CmonStringArray *arr, int index){
  if (!arr){
    return NULL;
  }

  if (index >= arr->len){
    return NULL;
  }

  if (index < 0){
    return NULL;
  }
  
  return arr->arr[index];
}


CmonString *cmon_str_arr_add_new_str(CmonStringArray *arr, const char*str){
  CmonString *newString; 
  if (!arr || !str){
    cmon_print_error(true, "cmon_add_string", "arr or string not provided");
    return NULL;
  }

  if (str[0] == '\0'){
    cmon_print_error(true, "cmon_add_string", "empty string");
    return NULL;
  }
  
  newString = cmon_string_new(str);
  if (!newString){
    cmon_print_error(true, "cmon_add_new_string", "could not make the new string");
    return NULL; 
  }

  if (arr->len + 1 > INITIAL_ARRAY_SIZE){
    return NULL;
  }

  arr->arr[arr->len] =  newString;
  arr->len++;
  return newString;
}


int cmon_str_arr_get_len(CmonStringArray *arr){
  return arr->len;
}
