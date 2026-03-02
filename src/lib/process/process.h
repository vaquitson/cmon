#pragma once

#include "c_int_array.h"
#include "config.h"

/* start the child procees */
int process_start_child(CmonConfig *conf);

/* send a sigterm to all the pid's provide in the pid array */
int process_kill_subtree(CmonIntArray *pid_array);
