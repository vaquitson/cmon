#include <sys/inotify.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <signal.h>
#include <dirent.h>

#include "cmon_errors.h"
#include "cmon_inotify.h"
#include "cmon_print.h"
#include "cmon_config.h"
#include "cmon_int_array.h"
#include "cmon_string_array.h"

#define INO_MASK IN_MODIFY


// starts the child process
int cmon_start_child_process(char *exe, char **argv){
  cmon_print_msg_to_user("Starting child process");
  pid_t pid;
  int rc;
   
  pid = fork();
  switch (pid){
    case 0:
      rc = execvp(exe, argv);
      if (rc == -1){
        int err = errno;
        cmon_print_errno_error(true, "cmon_start_child_process", err, "exec terminated incorrectly");
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


// open a inotify instance
int cmon_open_ino_fd(){
  int fd;
  fd = inotify_init();
  if (fd < 0){
    cmon_print_error(true, "open_inotify_fd", "inotify could not be instanciated");
    return -1;
  }
  return fd;
}


// watch the test directory
int cmon_ino_watch_dir(int inFd, char *path, unsigned long mask){
  int wd; // watch descriptor
  wd = inotify_add_watch(inFd, path, mask);
  if (wd < 0){
    cmon_print_error(true, "cmon_watch_dir", "The path could not be watched");
    return -1;
  }
  return wd;
}


// [FIXME] not if theis should retunr the event or waht : (
void cmon_procees_events(size_t readSize, char *buff, pid_t pgId, char *exe, char **argv){
  static const struct inotify_event *event;
  int rc;

  for (char *ptr = buff; ptr < buff + readSize; ptr += sizeof(struct inotify_event) + event->len){
    event = (const struct inotify_event *) ptr;
    printf("event name: %s\n", event->name);
    if (event->mask & IN_MODIFY) {
      printf("mod\n");
      //cmon_print_error(true, "cmon_start_child_process", "exec terminated incorrectly");
      rc = killpg(pgId, SIGUSR2);
      if (rc == -1){
        cmon_print_errno_error(true, "cmon_procees_events", errno, "kill faild");
      }
      //*subPPid = cmon_start_child_process(exe, argv);
    }
  }
}


// recursive funtion that adds nested directories to inotify watch dirs
void cmon_ino_recursive_dir_add(CmonConfig *conf, int inoFd, CmonString *path, CmonIntArray *wdArr){
  DIR *dir;
  struct dirent *dirEntry;
  CmonString *entryPath;
  CmonString *newPath;
  int wd;

  dir = opendir(cmon_str_get(path));
  errno = 0;

  while ((dirEntry = readdir(dir)) != NULL){
    entryPath = cmon_str_new_from_char_arrs(cmon_str_get(path), dirEntry->d_name, NULL);
    switch (dirEntry->d_type){
      case DT_DIR:
        if (strcmp(dirEntry->d_name, "..") == 0 || strcmp(dirEntry->d_name, ".") == 0){
          continue;
        }
        if (!cmon_str_arr_find(&conf->ignoreDirs, entryPath)){
          wd = cmon_ino_watch_dir(inoFd, cmon_str_get(entryPath), INO_MASK);
          printf("added: %s\n", cmon_str_get(entryPath));
          wdArr = cmon_int_arr_add(wdArr, wd);
          newPath = cmon_str_new_from_char_arrs(cmon_str_get(entryPath), "/", NULL);  
          cmon_ino_recursive_dir_add(conf, inoFd, newPath, wdArr);
        };
    }
    free(entryPath);
  }
  free(path);
  if (errno != 0){
    cmon_print_errno_error(true, "cmon_recursive_directorie_adding", errno, "an error ocurre");
    return;
  }
}

