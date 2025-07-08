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


// this functions add a new char array to the CmonStringArray.
// this create a new heap allocated CmonString with the content of str
// on error returnr NULL
CmonString *cmon_str_arr_add_new_char_arr(CmonStringArray *arr, char* str){
  CmonString *newString; 
  if (!arr || !str){
    cmon_print_error(true, "cmon_add_string", "arr or string not provided");
    return NULL;
  }

  if (str[0] == '\0'){
    cmon_print_error(true, "cmon_add_string", "empty string");
    return NULL;
  }
  
  newString = cmon_str_new(str);
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

CmonString *cmon_str_arr_add_from_str(CmonStringArray *arr, CmonString *str){
  if (!arr || !str){
    cmon_print_error(true, "cmon_str_arr_add_from_strs", "arr or string not provided");
    return NULL;
  }
   if (arr->len + 1 > INITIAL_ARRAY_SIZE){
      return NULL; 
   }

   arr->arr[arr->len] = str;
   arr->len++;
   return str;
}



CmonString *cmon_str_arr_find(CmonStringArray *arr, CmonString *str){
  for (int i = 0; i < cmon_str_arr_len(arr); i++){
    if (cmon_str_cmp(str, cmon_str_arr_get_str(arr, i))){
      return cmon_str_arr_get_str(arr, i);
    }
  }
  return NULL;
}


int cmon_str_arr_free(CmonStringArray *arr){
  if (!arr){
    cmon_print_error(true, "cmon_str_arr_free", "the CmonStringArray is NULL");
    return 1; 
  }
  
  for(int i = 0; i < arr->len; i++){
    free(arr->arr[i]);
  }
  return 0;
}

