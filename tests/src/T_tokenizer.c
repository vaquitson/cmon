#include "tokenizer.h"

#include <stdio.h>
#include <string.h>
#include <errno.h>

int test1(){
  FILE *config = fopen("/home/narval/programing/proyects/cmon/docs/config_exp", "r");
  if (config == NULL){ 
    printf("xd: %s\n", strerror(errno));
  }

  struct TokenArr *tokArr = token_arr_new();
  
  config_tokenizer(tokArr, config);
  token_arr_print(tokArr); 
  token_arr_free(tokArr);
  return 0;
}

int main(){
  printf("hello\n");
  test1();
  return 0;
}

