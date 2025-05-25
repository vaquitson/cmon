/*
 * This utils is needed because we could not kill subproecesses of our child
 * process. So we need to know the PIDs of the processes under the main process
 * we are going to use this command to get the pid's of the child processes
 * pstree -p -n [child pid]
*/

#pragma once

#include <unistd.h>
#include "cmon_int_array.h"

/*
 * this function gets a new heap allocated CmonIntArray with
 * the PID'S of the child processes tree including the child PID.
*/
CmonIntArray *ps_tree_get_int_arr(pid_t childPid);


