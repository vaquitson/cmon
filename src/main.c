#include <stdio.h>
#include <unistd.h>
#include <wait.h>

#include <sys/inotify.h>
#include <stdio.h>
#include <unistd.h>
#include <poll.h>

// open a inotify instance
int open_inotify_fd(){
  int fd;
  fd = inotify_init();
  if (fd < 0){
    printf("inotify could not be instanciated\n");
    return -1;
  }
  return fd;
}


// watch the test directory
int watch_dir(int inFd, char *path, unsigned long mask){
  int wd; // watch descriptor
  wd = inotify_add_watch(inFd, path, mask);
  if (wd < 0){
    printf("The path could not be watched\n");
    return -1;
  }
  return wd;
}


// starts the child process, wiich is a node server
int start_child_process(){
  pid_t pid;
  pid = fork();
  if (pid == 0){
    // the second argument is the name of the process, and the next is the parameter
    execlp("node", "juanpablosegundo", "/home/narval/programing/learning/inotify/inotify_creating_new_precees/test.js", NULL);
    return 0;
  } else if (pid < 0){
    return -1;
  } else {
    return pid;
  }
}


void procees_events(size_t readSize, char *buff, pid_t *subPPid){
  static const struct inotify_event *event;

  for (char *ptr = buff; ptr < buff + readSize; ptr += sizeof(struct inotify_event) + event->len){
    event = (const struct inotify_event *) ptr;
    printf("event: %s\n", event->name);
    if (event->mask & IN_MODIFY) {
      kill(*subPPid, SIGKILL);
      *subPPid = start_child_process();  
    }
  }
}


int main(){
  int inotifyFd;
  int watchFileList[2] = {0};
  unsigned long mask =  IN_MODIFY;
  const struct inotify_event *event;
  char buff[4096];
  size_t readSize;
  int ret;
  struct pollfd fds[1];
  pid_t subPPid;

  inotifyFd = open_inotify_fd();

  fds[0].fd = inotifyFd;
  fds[0].events = POLLIN;

  watchFileList[0] = watch_dir(inotifyFd, "/home/narval/programing/proyects/cmon/build/test.js", mask);

  subPPid = start_child_process(); 

  while (1){
    ret = poll(fds, 1, -1);

    if (ret > 0 && fds[0].revents & POLLIN){
      readSize = read(inotifyFd, buff, sizeof(buff));
      if (readSize <= 0){
        printf("Reading error\n");
      }
      procees_events(readSize, buff, &subPPid);
    } else {
      continue;
    }
  }
}
