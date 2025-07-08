#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <errno.h>

#include "ps_tree.h"
#include "cmon_errors.h"
#include "cmon_int_array.h"
#include "text_utils.h"

static pid_t ps_tree_exec(pid_t childPid, int pipeRead, int pipeWrite){
  int rc;
  char childPidBuff[10];
  pid_t pid;
  
  pid = fork();
  switch(pid){
    case 0:
      // alarge number to fit in the childPidBuff
      if (childPid > 100000000){
        cmon_print_error(true, "cmon_exec_ps_tree", "the pid is to large");
        return 1;
      }

      rc = sprintf(childPidBuff, "%d", childPid);
      if (rc <= 0){ 
        cmon_print_error(true, "cmon_exec_ps_tree", "something went wrong sprintf the pid");
        return 1;
      }

      close(pipeRead);
      dup2(pipeWrite, STDOUT_FILENO);
      
      rc = execlp("pstree", "pstree", "-p", "-n", childPidBuff, NULL);
      if (rc == -1){ 
        cmon_print_error(true, "cmon_exec_ps_tree", "the exec went wrong");
        return 1;
      }
      return 0;

    case -1:
      cmon_print_errno_error(true, "cmon_exec_ps_tree", errno, "the fork went wrong");
      return 1;
    
    default:
      close(pipeWrite);
      return pid;
  }
}


/* 
 * get the pid's from the pipe and added to the pid array
*/
CmonIntArray *_ps_tree_get_pids_from_pipe(int pipe_read_fd, CmonIntArray *pid_array){ 
  char buff[1024];
  ssize_t nBytes;

  char pid_buff[10];
  char *pid_buff_p;

  pid_buff_p = pid_buff;

  do {
    nBytes = read(pipe_read_fd, buff, 1024 - 1);
    if (nBytes){
      buff[nBytes] = '\0';
      for (int i = 0; i < nBytes; i++){
        if (char_is_numeric(buff[i])){
          *pid_buff_p = buff[i]; 
          pid_buff_p++;
        } else {
          if (pid_buff_p > pid_buff){
            *pid_buff_p = '\0';
            pid_array = cmon_int_arr_add(pid_array, atoi(pid_buff));
            pid_buff_p = pid_buff;
          }
        }
      }
    }
  } while (nBytes > 0);

  return pid_array;
} 


CmonIntArray *ps_tree_get_pid_arr(pid_t childPid){
  int pipes[2];
  int pipe_write_fd;
  int pipe_read_fd; 

  pid_t proc_pid;
  int proc_status;

  CmonIntArray *pid_array;

  pid_array = cmon_int_arr_new(50);

  // pipe setup
  if (pipe(pipes) != 0){
    cmon_print_errno_error(true, "cmon_ps_tree_get_int_arr", errno, "the pipe creatino faild");
  }
  pipe_read_fd = pipes[0];
  pipe_write_fd = pipes[1];

  // exec ps tree
  proc_pid = ps_tree_exec(childPid, pipe_read_fd, pipe_write_fd);

  // reed pipe 
  pid_array = _ps_tree_get_pids_from_pipe(pipe_read_fd, pid_array);
 
  while (true){
    waitpid(proc_pid, &proc_status, 0);
    if (WIFEXITED(proc_status)){
      break;
    } else if (WIFSIGNALED(proc_status)){
      break;
    }
  } 
  return pid_array;
}

