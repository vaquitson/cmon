#include <stdio.h>
#include <wait.h>
#include <stdbool.h>
#include <sys/inotify.h>
#include <stdio.h>
#include <unistd.h>
#include <poll.h>

#include "cmon_errors.h"
#include "inotify.h"
#include "process.h"
#include "config.h"
#include "logger.h"

#define CONFIG_FILE_NAME_LEN 11

int main(int argc, char **argv){
  int inotifyFd;
  CmonIntArray *wdArr = cmon_int_arr_new(100); // watch descriptor arr 
  log_write(LOG_INFO, "from main: The cmon_int array is initialized");

  int pollRetV;
  size_t readSize;
  char buff[4096];
  const struct inotify_event *event;
  struct pollfd fds[1];

  CmonConfig *conf;

  pid_t sub_pid;

  // config initializer
  conf = config_new();
  config_init(conf, argc, argv);
  log_write(LOG_INFO, "from main: The conf is initialized");
  
  inotifyFd = ino_init();
  ino_recursive_dir_add(conf, inotifyFd, cmon_str_copy(conf->cwd), wdArr);

  fds[0].fd = inotifyFd;
  fds[0].events = POLLIN;

  sub_pid = process_start_child(conf); 
  printf("parentPid %d\n", getpid());
  printf("childPid %d\n", sub_pid);

  config_print(conf);
  
  // [FIXME] this should be a parameter in config i guess
  // this sleep is for give time to the chilf child process 
  // to start all his sub processes
  sleep(1); 

  while (1){
    pollRetV = poll(fds, 1, -1);
    if (pollRetV > 0 && fds[0].revents & POLLIN){
      readSize = read(inotifyFd, buff, sizeof(buff));
      if (readSize <= 0){
        cmon_print_error(true, "main", "read size < 0 in reading the events");
        printf("Reading error\n");
      }
      ino_procees_events(conf, readSize, buff, &sub_pid);
    } else {
      continue;
    }
  }
}
