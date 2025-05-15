#include <stdio.h>
#include <stdlib.h>

#include "cmon_string.h"
#include "cmon_string_array.h"

int basic_test(){
  CmonString *string = cmon_str_new_from_char_arrs("gato", "zapato", "megalodon", NULL); 
  CmonString *string2 = cmon_str_new_from_char_arrs("perro", NULL); 
  CmonString *string3 = cmon_str_new_from_char_arrs("anguila", NULL); 

  printf("%s\t%d\n", string->string, string->len);

  printf("%s\t%d\n", string2->string, string2->len);

  printf("%s\t%d\n", string3->string, string3->len);

  free(string);
  free(string2);
  free(string3);
  return 0;
}

int new_string_from_multiple_str(){
  CmonString *string = cmon_str_new("gato"); 
  CmonString *string2 = cmon_str_new("perro"); 
  CmonString *string3 = cmon_str_new("anguila"); 
  CmonString *res;
  res = cmon_str_new_from_str(string, string2, string3, NULL);

  printf("%s\n", res->string);
  printf("%d\n", res->len); 

  free(string);
  free(string2);
  free(string3);
  free(res);
  return 0;
}

// this test should use valgrind to see if the memory of the amount of 
// memory of the string is correct
// this shuld use:
// usigned int 4 bytes
// char array 7 bytes
// null char 1 byte
int size_test(){
  CmonString *string = cmon_str_new_from_char_arrs("123", "4567", NULL); 
  return 0;
}


int string_array_basic_test(){
  CmonStringArray *arr = cmon_str_arr_new();
  cmon_str_arr_add_new_char_arr(arr, "perro");
  cmon_str_arr_add_new_char_arr(arr, "gato");
  cmon_str_arr_add_new_char_arr(arr, "unicornio");
  cmon_str_arr_add_new_char_arr(arr, "pez");
  

  CmonString *ptr;
  for (int i = 0; i < arr->len; i++){
    ptr = cmon_str_arr_get_str(arr, i); 
    printf("%s\n", cmon_str_get(ptr));
  }

  printf("%d\n", cmon_str_arr_len(arr));
  return 0;
}

int string_test_compare(){
  CmonString *str1 = cmon_str_new("hello");
  CmonString *str2 = cmon_str_new("hello");

  CmonString *str3 = cmon_str_new("perro");
  CmonString *str4 = cmon_str_new("gato");

  if (cmon_str_cmp(str1, str2)){
    printf("succes 1\n");
  } else {
    return 1;
  }

  if (cmon_str_cmp(str3, str4)){
    return 1;
  } else {
    printf("succes 2\n");
  }

  return 0;
}


int main (int argc, char *argv[]){
  string_array_basic_test();
}
