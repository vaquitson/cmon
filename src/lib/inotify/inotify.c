#include <sys/inotify.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <dirent.h>
#include <errno.h>

#include "cmon_errors.h"
#include "inotify.h"
#include "cmon_int_array.h"
#include "cmon_string_array.h"
#include "cmon_str_utils.h"
#include "logger.h"
#include "config.h"
#include "ps_tree.h"
#include "process.h"
#include "cmon_path.h"

#define INO_MASK IN_MODIFY

// open a inotify instance
int ino_init(void){
  int fd;
  fd = inotify_init();
  if (fd < 0){
    log_write(LOG_ERROR, "ino_init could initialize the inotify instance");
    exit(-1);
  }

  log_write(LOG_INFO, "the inotify instance has been correctly initialized");
  return fd;
}

// add a path to be watch by inotify. In success return the inotify watch descriptor and in 
// failure retruns -1
int _ino_watch_dir(int inFd, char *path, unsigned long mask){
  int wd;
  wd = inotify_add_watch(inFd, path, mask);
  if (wd < 0){
    log_write(LOG_ERROR, "ino_watch_dir faild; inotify could not add the path %s to be watched; errno -> ",
        path, 
        strerror(errno));
    return -1;
  }
  log_write(LOG_INFO, "the path %s has been correctly add to inotify watch list", path);
  return wd;
}


void ino_procees_events(CmonConfig *config, size_t readSize, char *buff, pid_t *child_pid){
  static const struct inotify_event *event;
  int rc = 0;
  CmonIntArray *pid_arr;

  for (char *ptr = buff; ptr < buff + readSize; ptr += sizeof(struct inotify_event) + event->len){
    event = (const struct inotify_event *) ptr;
    log_write(LOG_INFO, "envet from %s", event->name);

    if (event->mask & IN_MODIFY) {
      CmonPath *path = cmon_path_new_full_path(event->name);

      CmonString *find_res = cmon_str_arr_find(&config->ignoreFiles, 
          (CmonString *)path);
      if (find_res != NULL){
        log_write(LOG_INFO, "the file %s is in the ignore list", event->name);
        cmon_path_free(path);
        continue;
      }

      CmonString *find_ext_name_res = cmon_str_arr_find(&config->watchExtNames, 
          cmon_path_cast_ext_as_cmon_string(path));
      if (find_ext_name_res != NULL){
        log_write(LOG_INFO, "the file %s is not an extname we arent looking");
        cmon_path_free(path);
        continue;
      }
      
      pid_arr = ps_tree_get_pid_arr(*child_pid);
      rc = process_kill_subtree(pid_arr);
      waitpid(*child_pid, NULL, 0);

      free(pid_arr);
      if (rc != 0){
        log_write(LOG_WARNING, "cmon_procees_events could not kill the ps tree of the process %d", *child_pid);
      }

      *child_pid = process_start_child(config);
      log_write(LOG_INFO, "the process has been restarted", *child_pid);
      cmon_path_free(path);
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
          wd = _ino_watch_dir(inoFd, cmon_str_get(entryPath), INO_MASK);
          log_write(LOG_INFO, "the directory %s has been added", cmon_str_get(entryPath));
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

