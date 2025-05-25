#include <stdio.h>
#include <wait.h>
#include <stdbool.h>
#include <sys/inotify.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <poll.h>

#include "cmon_errors.h"
#include "inotify.h"
#include "process.h"
#include "config.h"

#define CONFIG_FILE_NAME_LEN 11

void _siguser2Cb(int sig){
  if (sig == SIGUSR2){
    printf("Restarting process ...");
  }
}

int main(int argc, char **argv){
  int inotifyFd;
  CmonIntArray *wdArr = cmon_int_arr_new(100);

  int pollRetV;
  size_t readSize;
  char buff[4096];
  const struct inotify_event *event;
  struct pollfd fds[1];

  CmonConfig *conf;
  struct CmonCommand *command;  

  pid_t sub_pid;

  signal(SIGUSR2, _siguser2Cb);

  // config initializer
  conf = config_new();
  command = cmon_parse_argv(argc, argv);
  config_init(conf);
  
  inotifyFd = cmon_open_ino_fd();

  fds[0].fd = inotifyFd;
  fds[0].events = POLLIN;

  cmon_ino_recursive_dir_add(conf, inotifyFd, cmon_str_copy(conf->cwd), wdArr);

  sub_pid = process_start_child(command->exe, command->argv); 

  printf("parentPid %d\n", getpid());
  printf("childPid %d\n", sub_pid);
  
  // [FIXME] this should be a parameter in config i guess
  sleep(1);

  while (1){
    pollRetV = poll(fds, 1, -1);
    if (pollRetV > 0 && fds[0].revents & POLLIN){
      readSize = read(inotifyFd, buff, sizeof(buff));
      if (readSize <= 0){
        cmon_print_error(true, "main", "read size < 0 in reading the events");
        printf("Reading error\n");
      }
      cmon_procees_events(readSize, buff, &sub_pid, command->exe, command->argv);
    } else {
      continue;
    }
  }
}
