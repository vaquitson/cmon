#include <stdlib.h>
#include <stdio.h>
#include "cmon_errors.h"
#include "cmon_string.h"

CmonString *cmon_string_new(char *string){
  char c; 
  unsigned int i = 0;
  while ((c = string[i]) != '\0'){
    i++; 
    if (i > MAX_STRING_LEN){
      cmon_print_error(true, "cmon_string_new", "the string is larger than the max valid len");
      exit(1);
    }
  }
  printf("%d\n", i);
  
  CmonString *newString = malloc(sizeof(CmonString) + i + 1);
  newString->stringLen = i;
  return newString;
}
