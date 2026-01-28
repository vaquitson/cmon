#pragma once

#include <stdlib.h>

ssize_t _http_get_headders(int fd, char *buff, size_t buff_len, size_t *read_len);
