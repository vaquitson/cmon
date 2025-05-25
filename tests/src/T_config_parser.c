#include "tokenizer.h"
#include "config.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int test1(){
  CmonConfig *conf = config_new();

  CmonString *cwd;
  CmonString *configFileName;
  CmonString *configFilePath;

  cwd = config_get_cwd();
  conf->cwd = cwd;

  configFileName = cmon_str_new(".config.cmon"); 
  configFilePath = cmon_str_new_from_str(cwd, configFileName, NULL);
  free(configFileName);

  FILE *config_file = config_open_file(configFilePath);

  struct TokenArr *tok_arr = token_arr_new(); 
  config_tokenizer(tok_arr, config_file);
  token_arr_print(tok_arr);

  config_parser(tok_arr, conf);
  token_arr_free(tok_arr);

  printf("hello\n");
  config_print(conf);
  return 0;
}


int test2(){
  CmonConfig *conf = config_new();
  config_init(conf);
  config_print(conf);

  config_free(conf);
  return 0;
}

int main(){
  test2();
  return 0;
}

