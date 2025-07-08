#include <sys/inotify.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <dirent.h>

#include "cmon_errors.h"
#include "inotify.h"
#include "cmon_int_array.h"
#include "cmon_string_array.h"
#include "cmon_str_utils.h"

#include "config.h"
#include "ps_tree.h"
#include "process.h"

#define INO_MASK IN_MODIFY

// open a inotify instance
int ino_init(){
  int fd;
  fd = inotify_init();
  if (fd < 0){
    cmon_print_error(true, "open_inotify_fd", "inotify could not be instanciated");
    return -1;
  }
  return fd;
}


int ino_watch_dir(int inFd, char *path, unsigned long mask){
  int wd;
  wd = inotify_add_watch(inFd, path, mask);
  if (wd < 0){
    cmon_print_error(true, "cmon_watch_dir", "The path could not be watched");
    return -1;
  }
  return wd;
}


// [FIXME] not if theis should retunr the event or waht : (
void ino_procees_events(CmonConfig *config, size_t readSize, char *buff, pid_t *child_pid){
  static const struct inotify_event *event;
  int rc = 0;
  CmonIntArray *pid_arr;

  for (char *ptr = buff; ptr < buff + readSize; ptr += sizeof(struct inotify_event) + event->len){
    event = (const struct inotify_event *) ptr;
    printf("event name: %s\n", event->name);
    if (event->mask & IN_MODIFY) {

      CmonString *string_event_name = cmon_str_new_from_char_arrs(config->cwd->string ,event->name, NULL); 
      CmonString *find_res = cmon_str_arr_find(&config->ignoreFiles, string_event_name); 
      free(string_event_name);
      if (find_res != NULL){
        free(find_res);
        return;
      }

      char *file_ext_name = get_ext_name_from_cmon_string(string_event_name);
      printf("ext name %s\n", file_ext_name);
      CmonString *string_ext_name = cmon_str_new(file_ext_name); 
      CmonString *find_ext_name_res = cmon_str_arr_find(&config->watchExtNames, string_ext_name);
      free(string_ext_name);
      if (find_ext_name_res == NULL){
        return; 
      } 
      free(find_ext_name_res);
      pid_arr = ps_tree_get_pid_arr(*child_pid);
      process_kill_subtree(pid_arr);
      waitpid(*child_pid, NULL, 0);
      free(pid_arr);
      if (rc == -1){
        cmon_print_errno_error(true, "cmon_procees_events", errno, "kill faild");
      }
      *child_pid = process_start_child(config);
    }
  }
}


// recursive funtion that adds nested directories to inotify watch dirs
void ino_recursive_dir_add(CmonConfig *conf, int inoFd, CmonString *path, CmonIntArray *wdArr){
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
        if (strcmp(dirEntry->d_name, "..") == 0){
          continue;
        }
        else if (!cmon_str_arr_find(&conf->ignoreDirs, entryPath)){
          wd = ino_watch_dir(inoFd, cmon_str_get(entryPath), INO_MASK);
          printf("added: %s\n", cmon_str_get(entryPath));
          wdArr = cmon_int_arr_add(wdArr, wd);
          newPath = cmon_str_new_from_char_arrs(cmon_str_get(entryPath), "/", NULL);  
          if (strcmp(dirEntry->d_name, ".") != 0){
            ino_recursive_dir_add(conf, inoFd, newPath, wdArr);
          }
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

