#pragma once

#include "cmon_int_array.h"

/* start the child procees */
int process_start_child(char *exe, char **argv);

/* send a sigterm to all the pid's provide in the pid array */
int process_kill_subtree(CmonIntArray *pid_array);
