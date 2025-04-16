#include <stdio.h>
#include "cmon_print.h"

void cmon_print_msg_to_user(char *msg){
  printf("\x1b[38;5;123m%s\x1b[39m\n", msg);
}
