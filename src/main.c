#include <stdio.h>
#include <unistd.h>
#include <wait.h>
#include <stdbool.h>
#include <sys/inotify.h>
#include <stdio.h>
#include <unistd.h>
#include <poll.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "cmon_errors.h"
#include "config.h"
#include "cmon_print.h"

#define CONFIG_FILE_NAME_LEN 11


// open a inotify instance
int cmon_open_inotify_fd(){
  int fd;
  fd = inotify_init();
  if (fd < 0){
    cmon_print_error(true, "open_inotify_fd", "inotify could not be instanciated");
    return -1;
  }
  return fd;
}


// watch the test directory
int cmon_watch_dir(int inFd, char *path, unsigned long mask){
  int wd; // watch descriptor
  wd = inotify_add_watch(inFd, path, mask);
  if (wd < 0){
    cmon_print_error(true, "watch_dir", "The path could not be watched");
    return -1;
  }
  return wd;
}


// starts the child process, wiich is a node server
int cmon_start_child_process(char *exe, char **argv){
  cmon_print_msg_to_user("Starting child process");
  pid_t pid;
  int rc;
   
  pid = fork();
  if (pid == 0){
  rc = execvp(exe, argv);
    if (rc == -1){
      int err = errno;
      cmon_print_errno_error(true, "cmon_start_child_process", err, "exec terminated incorrectly");
      exit(errno);
    }
    return 0;
  } else if (pid < 0){
    cmon_print_error(true, "start_child_process", "something went wrong in forking");
    return -1;
  } else {
    return pid;
  }
}


void cmon_procees_events(size_t readSize, char *buff, pid_t *subPPid, char *exe, char **argv){
  static const struct inotify_event *event;

  for (char *ptr = buff; ptr < buff + readSize; ptr += sizeof(struct inotify_event) + event->len){
    event = (const struct inotify_event *) ptr;
    printf("event: %s\n", event->name);
    if (event->mask & IN_MODIFY) {
      cmon_print_error(true, "cmon_start_child_process", "exec terminated incorrectly");
      kill(*subPPid, SIGKILL);
      *subPPid = cmon_start_child_process(exe, argv);  
    }
  }
}


int main(int argc, char **argv){
  int inotifyFd;
  int watchFileList[2] = {0};
  int ret;
  size_t readSize;
  pid_t subPPid;
  unsigned long mask = IN_MODIFY;
  char buff[4096];
  const struct inotify_event *event;
  struct pollfd fds[1];
  char *cwd;
  struct CmonCommand *command;  
  char *configFilePath;

  command = cmon_parse_argv(argc, argv);
  cwd = cmon_get_cwd();
  configFilePath = realloc(cwd, strlen(cwd) + CONFIG_FILE_NAME_LEN + 2);
  strcat(configFilePath, "/.config.cmon");
  
  printf("config path: %s\n", configFilePath);
  if (!configFilePath){
    cmon_print_error(true, "main", "something went wrong allocating memory for the config file path");
    exit(1);
  }

  FILE *configFile = cmon_open_config_file(configFilePath);
  struct CmonConfig *config = cmon_init_config();

  cmon_parse_config(configFile, config);
  printf("wtchneame len: %d\n", config->watchExtNamesLen);
  for (int i = 0; i < config->watchExtNamesLen; i++){
    printf("hoa\n");
    printf("extname: %s\n", config->watchExtNames[i]);
  }
  
  inotifyFd = cmon_open_inotify_fd();

  fds[0].fd = inotifyFd;
  fds[0].events = POLLIN;

  watchFileList[0] = cmon_watch_dir(inotifyFd, "/home/narval/programing/proyects/cmon/build/test.js", mask);

  subPPid = cmon_start_child_process(command->exe, command->argv); 

  while (1){
    ret = poll(fds, 1, -1);
    if (ret > 0 && fds[0].revents & POLLIN){
      readSize = read(inotifyFd, buff, sizeof(buff));
      if (readSize <= 0){
        cmon_print_error(true, "main", "read size < 0 in reading the events");
        printf("Reading error\n");
      }
      cmon_procees_events(readSize, buff, &subPPid, command->exe, command->argv);
    } else {
      continue;
    }
  }
}
