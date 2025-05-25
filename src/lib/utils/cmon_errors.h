#pragma once
#include <stdbool.h>

// log files to temp/log
int cmon_log(const char *status, const char *functionName, const char *msg);

// print errors to werever fd you want
int cmon_print_error(bool log, const char *functionName, const char *msg);

// print the corresponding message and the message from errno with the  strerror string
int cmon_print_errno_error(bool log, const char *functionName, const int err, const char *msg);

