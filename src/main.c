#include <stdio.h>
#include <unistd.h>
#include <wait.h>
#include <stdbool.h>
#include <sys/inotify.h>
#include <stdio.h>
#include <unistd.h>
#include <poll.h>
#include <stdlib.h>

#include "cmon_errors.h"
#include "config.h"

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
int cmon_start_child_process(){
  pid_t pid;
  pid = fork();
  if (pid == 0){
    execlp("node", "juanpablosegundo", "/home/narval/programing/learning/inotify/inotify_creating_new_precees/test.js", NULL);
    return 0;
  } else if (pid < 0){
    cmon_print_error(true, "start_child_process", "something went wrong in forking");
    return -1;
  } else {
    return pid;
  }
}


void cmon_procees_events(size_t readSize, char *buff, pid_t *subPPid){
  static const struct inotify_event *event;

  for (char *ptr = buff; ptr < buff + readSize; ptr += sizeof(struct inotify_event) + event->len){
    event = (const struct inotify_event *) ptr;
    printf("event: %s\n", event->name);
    if (event->mask & IN_MODIFY) {
      kill(*subPPid, SIGKILL);
      *subPPid = cmon_start_child_process();  
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

  cwd = cmon_get_cwd();
  printf("%s\n", cwd);


  inotifyFd = cmon_open_inotify_fd();

  fds[0].fd = inotifyFd;
  fds[0].events = POLLIN;

  watchFileList[0] = cmon_watch_dir(inotifyFd, "/home/narval/programing/proyects/cmon/build/test.js", mask);

  subPPid = cmon_start_child_process(); 

  while (1){
    ret = poll(fds, 1, -1);

    if (ret > 0 && fds[0].revents & POLLIN){
      readSize = read(inotifyFd, buff, sizeof(buff));
      if (readSize <= 0){
        cmon_print_error(true, "main", "read size < 0 in reading the events");
        printf("Reading error\n");
      }
      cmon_procees_events(readSize, buff, &subPPid);
    } else {
      continue;
    }
  }
}
