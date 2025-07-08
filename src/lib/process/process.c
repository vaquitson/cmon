#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <stdlib.h>

#include "process.h"
#include "cmon_errors.h"
#include "cmon_print.h"
#include "cmon_int_array.h"

int process_start_child(CmonConfig *conf){
  cmon_print_msg_to_user("Starting child process");
  pid_t pid;
  int rc; 
  pid = fork();
  switch (pid){
    case 0:
      rc = execvp(cmon_str_get(conf->exe), conf->argv);
      if (rc == -1){
        int err = errno;
        cmon_print_errno_error(true, "process_start_child", err, "exec terminated incorrectly");
        exit(errno);
      }
      return 0;
    case -1:
      cmon_print_error(true, "start_child_process", "something went wrong in forking");
      return -1;
    default:
      return pid;
  }
}

int process_kill_subtree(CmonIntArray *pid_array){
  if (!pid_array){
    cmon_print_error(true, "process_kill_subtree", "no pid array provided");
    return 1;
  }

  if (pid_array->len == 0){
    cmon_print_error(true, "process_kill_subtree", "the pid array is empty");
    return 1;
  }
  
  for (int i = pid_array->len-1; i >= 0; i--){
    if (kill(pid_array->arr[i], SIGTERM) != 0){
      cmon_print_error(true, "process_kill_subtree", "the signal has not been setn");
    }
  }
  return 0;
 
}
