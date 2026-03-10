#pragma once
#include "c_utils_string.h"



/*
 * This function returns a pointer to the substring containing the file extension
 * of the provided file name. If the file name does not contain an extension,
 * NULL is returned.
 * The returned pointer refers to memory within the original string and should not be freed.
 */
char *get_ext_name_from_cmon_string(CmonString *file_name);
