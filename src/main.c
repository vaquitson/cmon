#include <stdio.h>
#include <wait.h>
#include <stdbool.h>
#include <sys/inotify.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <poll.h>
#include <stdlib.h>

#include "cmon_errors.h"
#include "cmon_config.h"
#include "cmon_inotify.h"
#include "cmon_int_array.h"
#include "cmon_ps_tree.h"

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

  CmonConfig *config;

  CmonString *cwd;
  CmonString *configFileName;
  CmonString *configFilePath;

  struct CmonCommand *command;  

  pid_t subPPid;
  pid_t pgId;

  signal(SIGUSR2, _siguser2Cb);

  config = cmon_config_new();
  command = cmon_parse_argv(argc, argv);

  cwd = cmon_get_cwd();
  config->cwd = cwd;

  configFileName = cmon_str_new(".config.cmon"); 
  configFilePath = cmon_str_new_from_str(cwd, configFileName, NULL);
  free(configFileName);

  FILE *configFile = cmon_open_config_file(configFilePath);

  cmon_parse_config(configFile, config);
  
  inotifyFd = cmon_open_ino_fd();

  fds[0].fd = inotifyFd;
  fds[0].events = POLLIN;

  cmon_ino_recursive_dir_add(config, inotifyFd, cmon_str_copy(config->cwd), wdArr);

  pgId = getpgid(0);

  subPPid = cmon_start_child_process(command->exe, command->argv); 

  printf("parentPid %d\n", getpid());
  printf("childPid %d\n", subPPid);
  printf("process group %d\n", pgId);
  
  // [FIXME] this should be a parameter in config i guess
  sleep(1);
  cmon_ps_tree_get_int_arr(subPPid);

  while (1){
    pollRetV = poll(fds, 1, -1);
    if (pollRetV > 0 && fds[0].revents & POLLIN){
      readSize = read(inotifyFd, buff, sizeof(buff));
      if (readSize <= 0){
        cmon_print_error(true, "main", "read size < 0 in reading the events");
        printf("Reading error\n");
      }
      cmon_procees_events(readSize, buff, pgId, command->exe, command->argv);
    } else {
      continue;
    }
  }
}
