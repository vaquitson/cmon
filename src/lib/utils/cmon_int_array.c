#include <stdlib.h>

#include "cmon_int_array.h"
#include "cmon_errors.h"


// this functions initalizes and allocate memory for a new 
// CmonIntArray structure
// if something went wrong NULL is returnd
// The initialSize param indicate the amount of ints thath should
// contain the array at first. This size could grow if the array gets small
CmonIntArray *cmon_int_arr_new(unsigned int initialSize){ 
  if (initialSize == 0){
    cmon_print_error(true, "cmon_int_arr_new", "could not make a int array of size 0");
    return NULL;
  }
  CmonIntArray *intArr = (CmonIntArray *)malloc(sizeof(CmonIntArray) + (sizeof(int) * initialSize));
  if (!intArr){
    cmon_print_error(true, "cmon_int_array", "could not allocate memory for the CmonIntArray");
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
    cmon_print_error(true, "cmon_int_array_add", "no CmonIntArray was provided");
    return NULL;
  }

  if (arr->len + 1 > MAX_INT_ARRAY_SIZE){
    cmon_print_error(true, "cmon_int_array_add", "the array is larger than the maximum array size");
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

