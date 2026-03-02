#pragma once
#define MAX_INT_ARRAY_SIZE 5000
#define MEMORY_INCREMENT 15

#include <stdlib.h>

/*
 * This util privide a resisable int array 
 * that could hold at max MAX_INT_ARRAY_SIZE integers
*/

typedef struct {
  size_t len; 
  size_t size;
  int arr[];

} CmonIntArray;

// Creates a new heap allocated CmonIntArray. 
// the caller of the function is responisble for realeasing the memory
CmonIntArray *cmon_int_arr_new(unsigned int initialSize);

// add a new int to the int CmonIntArray.
// in success return a pointer to the arr, else return NULL
CmonIntArray *cmon_int_arr_add(CmonIntArray *arr, int add);

/* print the array to std */
void cmon_int_array_print(CmonIntArray *arr);
