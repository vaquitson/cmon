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


/* init Inotify and return his FD */
int ino_init();

/* set a single diretory to be watch with the 
 * inotify_add_watch function and retryn the 
 * watch descriptor
*/
int ino_watch_dir(int inFd, char *path, unsigned long mask);

/* process a file notify event */
void ino_procees_events(CmonConfig *config, size_t readSize, char *buff, pid_t *child_pid);


/* 
 * recursibly add the directories to be watch and filter by the provided
 * data in the CmonConfig
*/
void ino_recursive_dir_add(CmonConfig *conf, int inoFd, CmonString *path, CmonIntArray *wdArr);

