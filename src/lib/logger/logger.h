#pragma once

enum {
  DEBUG,
  INFO,
  WARNING,
  ERROR
};

enum {
  CONFIG,
  MAIN,
  INOTIFY,
  PROCESS,
  PS_TREE,
  UTILS,
};


void log_init();


void log_log(int level, int module, char *msg);
