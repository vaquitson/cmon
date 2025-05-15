#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <errno.h>

#include "cmon_errors.h"
#include "cmon_ps_tree.h"
#include "cmon_int_array.h"


static pid_t _cmon_exec_ps_tree(pid_t childPid, int pipeRead, int pipeWrite){
  int rc;
  char childPidBuff[10];
  pid_t pid;
  
  pid = fork();
  switch(pid){
    case 0:
      if (childPid > 1000000){
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


CmonIntArray *cmon_ps_tree_get_int_arr(pid_t childPid){
  int pipes[2];
  int pipeWriteFd;
  int pipeReadFd; 

  char buff[1024];
  ssize_t nBytes;

  pid_t procPid;
  int procStatus;

  int rc;

  rc = pipe(pipes);
  if (rc != 0){
    cmon_print_errno_error(true, "cmon_ps_tree_get_int_arr", errno, "the pipe creatino faild");
  }
  pipeReadFd = pipes[0];
  pipeWriteFd = pipes[1];

  procPid = _cmon_exec_ps_tree(childPid, pipeReadFd, pipeWriteFd);

  do {
    nBytes = read(pipeReadFd, buff, 1024 - 1);
    if (nBytes){
      buff[nBytes] = '\0';
      printf("%s", buff);
    }
  } while (nBytes > 0);
  
  while (true){
    waitpid(procPid, &procStatus, 0);
    if (WIFEXITED(procStatus)){
      break;
    } else if (WIFSIGNALED(procStatus)){
      break;
    }
  }
  
  return NULL;
}


void _cmon_parse_ps_tree_pids(){
}


