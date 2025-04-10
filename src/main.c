#include <stdio.h>
#include <sys/inotify.h>
#include <signal.h>
#include <stdbool.h>
#include <stdlib.h>
#include <poll.h>
#include <unistd.h>
#include <errno.h>
#include "cmon_errors.h"

bool keepRunning = 1;

void termination_cb(int sigNum){
  printf("ended\n");
  keepRunning = 0;
}

void handle_events(int inFd, int *wd, int argc, char **arg){
  printf("event handeling");
  char buff[4096];
  // __attribute__ ((aligned(__alignof__(struct inotify_event))));

  const struct inotify_event *event;
  ssize_t size;
  ssize_t u = 1;

  while (1){
    size = read(inFd, buff, sizeof(buff));
    // erno again is set wen the read dosent find enithing to read
    // but is not an error
    if (size == -1 && errno != EAGAIN){
      cmon_print_error(true, "handle_events", "canot read the event");
      return;
    }

    if (size <= 0){
      cmon_print_error(true, "handle_events", "the read function return an error");
      printf("size %zu\n", u);
      return;
    }
    
    // loop through the events in the buffer
    for (char *ptr = buff; ptr < buff; ptr += sizeof(struct inotify_event) + event->len){
      event = (const struct inotify_event *) ptr; 
 
      if (event->mask & IN_OPEN){
        printf("IN_OPEN: ");

      } else if (event->mask & IN_CLOSE_NOWRITE){
        printf("IN_CLOSE_NOWRITE: ");

      } else if (event->mask & IN_CLOSE_WRITE){
        printf("IN_CLOSE_WRITE: ");
      } else if (event->mask & IN_MODIFY){
        printf("IN_MODIFY: ");
      }
    }
  }
}


int main(int argc, char **argv){
  char testPath[] = "/home/narval/programing/proyects/cmon/test_file.txt";
  unsigned short int inotifyFlags = IN_NONBLOCK;
  struct sigaction terminationAction = {0};
  int inFd;
  int pollNum;
  int *watchDirectories;
  nfds_t nfds = 2;
  struct pollfd fds[nfds];
  const uint32_t eventWatchMask = IN_MODIFY | IN_OPEN | IN_ACCESS;

  terminationAction.sa_handler = termination_cb; 
  terminationAction.sa_flags = 0;

  if (sigaction(SIGINT, &terminationAction, NULL) != 0){
    cmon_print_error(true, "main", "could not assign the termination cb to SIGINT"); 
    return -1;
  } 
  
  watchDirectories = calloc(1, sizeof(int)); 
  if (!watchDirectories){
    cmon_print_error(true, "main", "could not allocate size for the watchDirectories array");
    return -2;
  }

  inFd = inotify_init1(inotifyFlags);

  fds[0].fd = STDIN_FILENO;
  fds[0].events = POLLIN;

  fds[1].fd = inFd;
  fds[1].events = POLLIN;
  
  // traking files
  watchDirectories[0] = inotify_add_watch(inFd, testPath, eventWatchMask);
  if (watchDirectories[0] == -1){
    cmon_print_error(true, "main", "coudl not watch the dir");
  }

  printf("Listening for events.\n");
  while (keepRunning){ 
    pollNum = poll(fds, nfds, -1);
    printf("hello");
    if (pollNum > 0){
      if (fds[1].revents & POLLIN){
        printf("FILEE\n");
        handle_events(inFd, watchDirectories, -1, NULL);
      }
    }
  }

  return 0;
}
