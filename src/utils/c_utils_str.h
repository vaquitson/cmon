#pragma once

#include <stdlib.h>

int c_u_str_copy_n(const char *src, const size_t n, char *dst, const size_t buf_size);
ssize_t c_u_str_find_pattern(const char *p, size_t p_len, const char *t, size_t t_len);
void c_u_str_print_n(const char *src, const size_t n);

