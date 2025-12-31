#include "cmon_string.h"
#include "cmon_path.h"
#include "test_macros.h"

#include <string.h>
#include <stdio.h>

int test_test_1(){ 
  char path_1[] = "/home/narval/programing";
  char path_2[] = "/home/narval.12/programing.c";

  CmonPath *path = cmon_path_new(path_1);
  if (strcmp(path->path, path_1) != 0){
    printf("test_test_1: the path representation is not correct, got %s, expect %s\n", path->path, path_1);
    return 1; 
  }

  if (path->path_len != strlen(path_1)){
    printf("test_test_1: the length is not correct, got %ld, expect %ld\n", path->path_len, strlen(path_1));
    return 1;
  }

  if (path->ext != NULL){
    printf("test_test_1: the path ext is not null\n");
    return 1;
  }


  if (path->ext_len != 0){
    printf("test_test_1: the path ext_len get %ld expect -1\n", path->ext_len);
    return 1;
  }
  
  CmonString *cmon_string = cmon_str_new(path_1);
  CmonString *path_as_string = (CmonString *)path;
  if (cmon_str_cmp(cmon_string, path_as_string) != true){
    printf("test_test_1: the path struct is not correctly casted to CmonString");
    return 1; 
  };
  
  free(cmon_string->string);
  free(cmon_string);
  cmon_path_free(path);

  CmonPath *file_path = cmon_path_new(path_2);
  if (strcmp(file_path->path, path_2) != 0){
    printf("test_test_1: the path representation is not correct, got %s, expect %s\n", path->path, path_2);
    return 1; 
  }

  if (file_path->path_len != strlen(path_2)){
    printf("test_test_1: the length is not correct, got %ld, expect %ld\n", path->path_len, strlen(path_2));
    return 1;
  }

  if (strcmp(file_path->ext, ".c") != 0){
    printf("test_test_1: the path is not correct, get %s expect .c\n", file_path->ext);
    return 1;
  }

  if (file_path->ext_len != strlen(file_path->ext)){
    printf("test_test_1: the ext len is %ld expect %ld\n", file_path->ext_len, strlen(file_path->ext));
    return 1;
  }


  CmonString *ext_string = cmon_str_new(".c");
  CmonString *path_ext_as_string = cmon_path_get_ext_as_cmon_string(file_path);

  if (cmon_str_cmp(ext_string, path_ext_as_string) != true) {
    printf("test_test_1: the ext is not castable to a CmonString\n");
    return 1;
  }


  free(ext_string->string);
  free(ext_string);
  cmon_path_free(file_path);
  return 0;
}

int test_test_2(){
  char test_path[] = "/home/narval/programing/proyects/cmon/velocirraptor.py";
  size_t test_path_len = strlen(test_path);
  char test_ext[] =  ".py";
  size_t test_ext_len = strlen(test_ext);

  char test_path_2[] = "/home/narval/programing/proyects/cmon/velocirraptor";

  CmonPath *path = cmon_path_new_full_path("velocirraptor.py"); 
  if (strcmp(path->path, test_path) != 0){
    printf("test_test_2: get %s expecct %s\n", path->path, test_path);
    return 1;
  }

  if (strcmp(test_ext, path->ext) != 0){
    printf("test_test_2: get %s expecct %s\n", path->ext, test_ext);
    return 1;
  }

  if (path->path_len != test_path_len){
    printf("test_test_2: the path len is not correct, get %ld expect %ld\n", path->path_len, test_path_len);
    return 1;
  }

  if (path->ext_len != test_ext_len){
    printf("test_test_2: the ext len is not correct, get %ld expect %ld\n", path->ext_len, test_ext_len);
    return 1;
  }

  cmon_path_free(path);


  CmonPath *path2 = cmon_path_new_full_path("velocirraptor"); 
  if (path2->ext != NULL){
    printf("test_test_2: a path with no ext has a none null ext\n");
    return 1;
  }

  if (path2->ext_len != 0){
    printf("test_test_2: a path with no ext has a extlen of %ld\n", path2->ext_len);
    return 1;
  }

  cmon_path_free(path2);
  return 0;
}


int main() { 
  TEST(test_test_1(), "test 1");
  TEST(test_test_2(), "test 2");
  return 0;
}
