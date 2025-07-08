#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "cmon_str_utils.h"
#include "cmon_string.h"

char *get_ext_name_from_cmon_string(CmonString *file_name){
  for (int i = 0; i < file_name->len; i++){
    if (file_name->string[i] == '.'){
      if (i+1 < file_name->len){
        return (file_name->string)+i+1;
      }
    } 
  }
  return NULL;
}

