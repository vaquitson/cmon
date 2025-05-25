#include "ps_tree.h"

#include <stdio.h>
#include <stdlib.h>


int test_1(char *pid){
  CmonIntArray *pid_arr;
  pid_arr = ps_tree_get_int_arr(atoi(pid));
  for (int i = 0; i < pid_arr->len; i++){
    printf("pid: %d\n", pid_arr->arr[i]);
  }

  free(pid_arr); 
  return 0;
}

int main(int argc, char *argv[]){
  char *pid;
  if (argc > 0){
    pid = argv[1];
  } 
  else {
    printf("no args\n");
    return 1;
  }

  test_1(pid);
  
  return 0;
}
