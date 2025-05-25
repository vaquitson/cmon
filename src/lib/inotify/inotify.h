#pragma once

#include <stdlib.h>
#include <sys/inotify.h>
#include "config.h"
#include "cmon_int_array.h"

#define MAX_AMOUNT_OF_NESTED_DIRECTORIES 20

// [FIXME] this array probably should have a variable size 
typedef struct {
  int watchFilesList[MAX_AMOUNT_OF_NESTED_DIRECTORIES];
  int len;
} CmonInoWDirsArr;


int cmon_open_ino_fd();

int cmon_ino_watch_dir(int inFd, char *path, unsigned long mask);

void cmon_procees_events(size_t readSize, char *buff, pid_t *child_pid, char *exe, char **argv);

int process_start_child(char *exe, char **argv);

CmonInoWDirsArr *cmon_ino_w_dirs_arr_new();

void cmon_ino_recursive_dir_add(CmonConfig *conf, int inoFd, CmonString *path, CmonIntArray *wdArr);

CmonString *cmon_str_copy(CmonString *str);
