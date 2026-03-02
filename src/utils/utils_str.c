#include <stdio.h>

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
int u_str_copy_n_chars(char *src, size_t n, char *dst, size_t buf_size){
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


int u_str_insert_at(void){
  return 0;
}


