#include <stdlib.h>
#include <stdio.h>

#include "c_int_array.h"
#include "cmon_errors.h"
#include "logger.h"


// this functions initalizes and allocate memory for a new 
// CmonIntArray structure
// if something went wrong NULL is returnd
// The initialSize param indicate the amount of ints thath should
// contain the array at first. This size could grow if the array gets small
CmonIntArray *cmon_int_arr_new(unsigned int initialSize){ 
  if (initialSize == 0){
    log_write(LOG_WARNING, "from c_int_array_new: could not make a CmonIntArray of size 0");
    return NULL;
  }

  CmonIntArray *intArr = (CmonIntArray *)malloc(sizeof(CmonIntArray) + (sizeof(int) * initialSize));
  if (!intArr){
    log_write(LOG_ERROR, "from c_int_array_new: could not allocate memory");
    return NULL;
  }

  intArr->len = 0;
  intArr->size = initialSize;
  return intArr;
}


// this function adds a number to provided CmonIntArray arr
// the function return a pointer to the structure. If the new number does not
// fit in the array size, a new CmonIntArray is returnd and the old one is free
CmonIntArray *cmon_int_arr_add(CmonIntArray *arr, int add){
  CmonIntArray *newIntArray;
  if (!arr){
    log_write(LOG_WARNING, "from cmon_int_array_add: arr is null");
    return NULL;
  }

  if (arr->len + 1 > MAX_INT_ARRAY_SIZE){
    log_write(LOG_WARNING, "from cmon_int_array_add: the array is to large");
    return NULL;
  }

  if (arr->size < arr->len + 1){
    newIntArray = cmon_int_arr_new(arr->size + MEMORY_INCREMENT);
    newIntArray->len = arr->len + 1;
    for (int i = 0; i < arr->len; i++){
      newIntArray->arr[i] = arr->arr[i];
    }
    newIntArray->arr[arr->len] = add;
    free(arr);
    return newIntArray;
  } else {
    arr->arr[arr->len] = add;
    arr->len++;
    return arr;
  } 
}


void cmon_int_array_print(CmonIntArray *arr){
  for (int i = 0; i < arr->len; i++){
    printf("pid: %d\n", arr->arr[i]);
  }
}
