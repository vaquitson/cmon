#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#include "cmon_errors.h"
#include "cmon_string.h"


static unsigned int _char_arr_len(const char *charArr){
  unsigned int len= 0;
  while (*charArr != '\0'){
    if (len > 256){
      cmon_print_error(true, "_char_arr_len", "the char array is too large");
      return 0;
    }
    len++;
    charArr++;
  }
  return len;
}

// creates a new heap allocated CmonString combining all the char
// arrays provided.
// the caller of the fucntions is risponsivle for the 
// memory
CmonString *cmon_str_new_from_char_arrs(const char *charArr, ...){
  const char *buffArgs[MAX_AMOUNT_OF_STRINGS];

  va_list args;
  char *arg;
  unsigned int nArgs;

  unsigned int charBlockSize;

  char *ptr;
  CmonString *newStr;

  if (!charArr){
    cmon_print_error(true, "cmon_string_new", "the functions resive no strings");
    return NULL;
  }

  buffArgs[0] = charArr;
  charBlockSize = _char_arr_len(charArr);
  nArgs = 1;

  va_start(args, charArr);
  while ((arg = va_arg(args, char *)) != NULL){
    if (nArgs >= MAX_AMOUNT_OF_STRINGS){
      cmon_print_error(true, "cmon_string_new", "the amount of arguments exede the limit");
      return NULL;
    }
    charBlockSize += _char_arr_len(arg);
    buffArgs[nArgs] = arg;
    nArgs++;
  }

  newStr = (CmonString *)malloc(sizeof(CmonString) + charBlockSize + 1);
  if (!newStr){
    cmon_print_error(true, "cmon_string_new", "could not allocate memory for the new CmonString");
    return NULL;
  }

  ptr = newStr->string;
  for (int i = 0; i < nArgs; i++){
    arg = buffArgs[i];
    while (*arg != '\0'){
      *ptr = *arg;
      arg++;
      ptr++;
    }
  }
  *ptr = '\0';
  newStr->len = charBlockSize;
  return newStr;
}



// create a new cmon string form a singel char array
CmonString *cmon_str_new(char *charArr){
  return cmon_str_new_from_char_arrs(charArr, NULL);
}


CmonString *cmon_str_copy(CmonString *str){
  if (!str){
    return NULL;
  }
  return cmon_str_new(cmon_str_get(str));
}


// this functions coppy the string to a buffer;
// on error return a less than 0 number
int cmon_str_copy_to_char_arr(CmonString *str, char buff[], size_t buffSize){
  char c;
  int i = 0;

  if (!buff){
    cmon_print_error(true, "cmon_copy_string_to_char_arr", "the buff is not provided");
    return -1;
  }

  if (!str){
    cmon_print_error(true, "cmon_copy_string_to_char_arr", "the string was not provided");
    return -1;
  }

  if (str->len + 1 > buffSize){
    cmon_print_error(true, "cmon_copy_string_to_char_arr", "the buff size is smaller than the needed space");
    return -1;
  }

  while ((c = str->string[i]) != '\0'){
    buff[i] = c;
    i++;
  }

  buff[i] = '\0';
  return 0;
}


// this function check if the strings are equal, in that case return
// true, any other case return false
bool cmon_str_cmp(CmonString *a, CmonString *b){
  if (!a || !b){
    cmon_print_error(true, "cmon_str_cmp", "arguments are void");
    return false;
  }

  if (a->len != b->len){
    return false;
  }

  for (int i = 0; i < a->len; i++){
    if (a->string[i] != b->string[i]){
      return false;
    }
  }
  return true;
}


// this function take a null terminated set of string and return
// a new CmonString concatenating the strings pass.
// if string is NULL the function return NULL
CmonString *cmon_str_new_from_str(CmonString *string, ...){
  CmonString *strBuff[MAX_AMOUNT_OF_STRINGS];

  va_list args;
  CmonString *arg;
  unsigned int nArgs;

  unsigned int charBlockSIze;

  CmonString *newStr;
  char *ptr;

  if (!string){
    cmon_print_error(true, "cmon_str_new_from_strings", "the functions resive no strings");
    return NULL;
  }

  strBuff[0] = string;
  charBlockSIze = string->len;
  nArgs = 1;

  va_start(args, string);
  while((arg = va_arg(args, CmonString *)) != NULL){
    if (nArgs >= MAX_AMOUNT_OF_STRINGS ){
      cmon_print_error(true, "cmon_str_new_from_strings", "the amount of arguments exede the limit");
      return NULL;
    }
  charBlockSIze += arg->len;
    strBuff[nArgs] = arg;
    nArgs++;
  }
  
  newStr = (CmonString *)malloc(sizeof(CmonString) + charBlockSIze + 1);
  if (!newStr){
    cmon_print_error(true, "cmon_str_new_from_strings", "could not allocate memory for the new CmonString");
    return NULL;
  }

  ptr = newStr->string;
  for (int i = 0; i < nArgs; i++){
    arg = strBuff[i];
    for (int j = 0; j < arg->len; j++){
      *ptr = arg->string[j];
      ptr++;
    }
  }
  *ptr = '\0';
  newStr->len = charBlockSIze;
  return newStr;
}
