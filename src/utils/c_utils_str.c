#include <stdio.h>
#include <string.h>

#include "logger.h"

/*
 * Copies the first `n` bytes from `src` into the destination buffer `dest`
 * (capacity: `buf_size`). The data is treated as raw bytes and may include
 * '\0' bytes.
 *
 * Returns:
 *   0  on success (exactly `n` bytes are copied)
 *  -1  if `src` is NULL
 *  -2  if `dest` is NULL
 *  -3  if `n` is greater than `buf_size`
 */
int c_u_str_copy_n(char *src, size_t n, char *dst, size_t buf_size){
  if (src == NULL){
    return -1;
  }

  if (dst == NULL){
    return -2;
  }

  if (n > buf_size){
    return -3;
  }

  for (size_t i = 0; i < n; i++){
    dst[i] = src[i];
  }
  return 0; 
}

/*
 * Searches for the pattern `p` in the text `t`.
 *
 * Returns the zero-based index (as `ssize_t`) of the first character of the first
 * occurrence of `p` within `t`, or -1 if no match is found.
 */
ssize_t c_utils_find_pattern(const char *p, const char *t, const size_t t_len){
  if (p == NULL){
    log_write(LOG_ERROR, "from _find_patern: the pattern is NULL");
    return -1;
  }

  if (t == NULL){
    log_write(LOG_ERROR, "from _find_patern: the text is NULL");
    return -1;
  }

  size_t p_len = strlen(p);
  
  for (size_t s=0; s <= t_len - p_len; s++){
    for (size_t i=0; i < p_len; i++){
      if (t[s+i] != p[i]){
        break; 
      } else if (i == p_len - 1){
        return s;
      }
    }
  }
  return -1;
}


/*
 * print the first n chars of memory of Searches
*/
void c_u_str_print_n(char *src, size_t n){
  for (size_t i = 0; i < n; i++){
    putchar(src[i]);
  }
}
