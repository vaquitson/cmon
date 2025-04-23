#include <stdlib.h>
#include <string.h>

#include "cmon_errors.h"
#include "cmon_string.h"


CmonString *cmon_string_new(const char *string){
  char c; 
  int i = 0;
  while ((c = string[i]) != '\0'){
    i++; 
    if (i > MAX_STRING_LEN){
      cmon_print_error(true, "cmon_string_new", "the string is larger than the max valid len");
      return NULL;
    }
  }  
  CmonString *newString = malloc(sizeof(CmonString) + i + 1);

  strcpy(newString->string, string);
  newString->stringLen = i;

  return newString;
}
