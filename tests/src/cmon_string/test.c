#include <stdio.h>
#include "cmon_string.h"
#include "cmon_string_array.h"

int main (int argc, char *argv[]){
  CmonString *string = cmon_string_new("hola"); 
  printf("%s\n", string->string);
  printf("%d\n", string->stringLen);


  CmonStringArray *arr = cmon_str_arr_new();
  cmon_str_arr_add_new_str(arr, "perro");
  cmon_str_arr_add_new_str(arr, "gato");
  cmon_str_arr_add_new_str(arr, "unicornio");
  cmon_str_arr_add_new_str(arr, "pez");

  CmonString *ptr;
  for (int i = 0; i < arr->len; i++){
    ptr = cmon_str_arr_get_str(arr, i); 
    printf("%s\n", ptr->string);
  }

  printf("%d\n", cmon_str_arr_get_len(arr));
}
