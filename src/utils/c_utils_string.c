#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <logger.h>

#include "c_utils_string.h"

size_t _char_arr_len(const char *charArr){
  size_t len= 0;
  while (*charArr != '\0'){
    if (len > 256){
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
CmonString *cmon_str_new_from_char_arrs(char *charArr, ...){
  char *buffArgs[MAX_AMOUNT_OF_STRINGS*sizeof(char *)];

  va_list args;
  char *arg;
  unsigned int nArgs;

  size_t charBlockSize;

  char *ptr;
  CmonString *newStr;

  if (!charArr){
    return NULL;
  }

  buffArgs[0] = charArr;
  charBlockSize = _char_arr_len(charArr);
  nArgs = 1;

  va_start(args, charArr);
  while ((arg = va_arg(args, char *)) != NULL){
    if (nArgs >= MAX_AMOUNT_OF_STRINGS){
      return NULL;
    }
    charBlockSize += _char_arr_len(arg);
    buffArgs[nArgs] = arg;
    nArgs++;
  }
  
  newStr = (CmonString *)malloc(sizeof(CmonString));
  if (newStr == NULL){
    log_write(LOG_ERROR, "From cmon_str_new_from_char_arrs: coudl not allocate memory for the CmonString");
    return NULL;
  }

  newStr->string = malloc(sizeof(char)*charBlockSize+1);
  if (newStr->string == NULL){
    log_write(LOG_ERROR, "From cmon_str_new_from_char_arrs: coudl not allocate memory for the CmonString->string");
    return NULL;
  }

  if (!newStr){
    return NULL;
  }

  ptr = newStr->string;
  for (unsigned int i = 0; i < nArgs; i++){
    arg = buffArgs[i];
    while (*arg != '\0'){
      *ptr = *arg;
      arg++;
      ptr++;
    }
  }
  *ptr = '\0';
  newStr->size = charBlockSize;
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
    return -1;
  }

  if (!str){
    return -1;
  }

  if (str->size + 1 > buffSize){
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
    return false;
  }

  if (a->size != b->size){
    return false;
  }

  for (size_t i = 0; i < a->size; i++){
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
    return NULL;
  }

  strBuff[0] = string;
  charBlockSIze = string->size;
  nArgs = 1;

  va_start(args, string);
  while((arg = va_arg(args, CmonString *)) != NULL){
    if (nArgs >= MAX_AMOUNT_OF_STRINGS ){
      return NULL;
    }
  charBlockSIze += arg->size;
    strBuff[nArgs] = arg;
    nArgs++;
  }
  
  newStr = (CmonString *)malloc(sizeof(CmonString) + charBlockSIze + 1);
  if (!newStr){
    return NULL;
  }

  ptr = newStr->string;
  for (size_t i = 0; i < nArgs; i++){
    arg = strBuff[i];
    for (size_t j = 0; j < arg->size; j++){
      *ptr = arg->string[j];
      ptr++;
    }
  }
  *ptr = '\0';
  newStr->size = charBlockSIze;
  return newStr;
}
