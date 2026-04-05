#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

#include "c_utils_buffer.h"
#include "c_utils_str.h"

/*
 * Creates a new, empty buffer with an initial capacity of `cap` bytes.
 *
 * On success, returns a newly allocated (owned) `CmonBuffer *`.
 * On failure, returns NULL.
 */
CmonBuffer *c_u_buffer_empty(size_t cap){
  char *buf;
  CmonBuffer *c_buf;

  buf = malloc(cap*sizeof(char));
  if (buf == NULL){
    return NULL;
  }

  c_buf = malloc(sizeof(CmonBuffer));
  if (c_buf == NULL){
    free(buf);
    return NULL;
  }

  c_buf->buf = buf;
  c_buf->cap = cap;
  c_buf->len = 0;
  c_buf->avl = cap;

  return c_buf;
}


/*
 * Creates a new buffer initialized with the first `len` bytes of `src`.
 *
 * On success, returns a newly allocated (owned) `CmonBuffer *`.
 * On failure, returns NULL.
 */
CmonBuffer *c_u_buffer_new(const char *src, size_t len){
  CmonBuffer *c_buf;

  if (src == NULL){
    errno = EINVAL;
    return NULL;
  }

  c_buf = c_u_buffer_empty(len + C_U_BUFFER_INITIAL_EXTRA_SPACE);
 
  strncpy(c_buf->buf, src, len);
  c_buf->len= len;
  c_buf->avl -= len;
  
  return c_buf;
}


/* 
 * this function prints the provided buffer 
 * to stdout.
*/
void c_u_buffer_print(CmonBuffer *buf){
  if (buf == NULL){
    return;
  }

  for (size_t i = 0; i < buf->len; i++){
    if (buf->buf[i] == '\0'){
      putchar('|');
    } else {
      putchar(buf->buf[i]);
    }
  }
}


/*
 * Frees a `CmonBuffer`, including the underlying data buffer it owns.
 *
 * If `buf` is NULL, this function does nothing.
*/
void c_u_buffer_free(CmonBuffer *buf){
  if (buf != NULL){
    if (buf->buf != NULL){
      free(buf->buf);
    }
    free(buf);
  }
}


/*
 * Creates a non-owning view into a byte buffer.
 *
 * The view spans `len` bytes starting at offset `start` within `buf`, where
 * `buf` has a total size of `size` bytes.
 *
 * The returned view does not own the underlying buffer and must not modify it.
 * When you are done, free only the `CmonBufferView` structure (not `buf`).
 *
 * Returns a newly allocated `CmonBufferView *` on success, or NULL on failure.
*/
CmonBufferView *c_u_view_buffer_new(
    const char *buf,
    const size_t size, 
    size_t start, 
    ssize_t len){

  CmonBufferView *new_view;

  if (buf == NULL){
    return NULL;
  }

  if (size < len+start){
    return NULL;
  }

  new_view = malloc(sizeof(CmonBufferView)); 
  if (new_view == NULL){
    return NULL; 
 }
  
  if (len == -1){
    
  }

  if (len == C_U_BUFFER_VIEW_END){
    len = size - start;
  }

  new_view->buf = buf + start;
  new_view->len = len;

  return new_view;
}


/*
 * Shifts the contents of `buf` to the right by `n` positions,
 * starting at `index`.
 *
 * The byte at `index` and all subsequent bytes are moved `n`
 * positions forward, preserving the existing contents while
 * creating free space inside the buffer.
 *
 * This function is mainly intended for insertion-like operations
 * where space must be created without overwriting data.
 *
 * Shifting from the last valid position is not meaningful, since
 * there is no trailing data to move.
 *
 * Returns:
 *   0  on success.
 *  -1  if `buf` is NULL.
 *  -2  if `index` refers to the last valid position.
 *  -3  if a memory allocation error occurs.
 */
int c_u_buffer_shift_from(CmonBuffer *buf,
    size_t index, size_t n)
{
  size_t new_buf_cap;
  char *new_buf;
  char *new_buf_p;

  size_t components_len = 2;
  CmonBufferView components[components_len];

  if (buf == NULL){
    return -1;
  }

  if (index >= buf->len){
    return -2;
  } 

  if (buf->cap - buf->len < n){
    components[0].buf = buf->buf;
    components[0].len = index;

    components[1].buf = buf->buf + index;
    components[1].len = buf->len - index;

    new_buf_cap = buf->len + n + C_U_BUFFER_INITIAL_EXTRA_SPACE;
    new_buf = calloc(new_buf_cap, sizeof(char));
    if (new_buf == NULL){
      return -3;
    }

    buf->cap = new_buf_cap;
    buf->len = 0;
    new_buf_p = new_buf;

    for (size_t i = 0; i < components_len; i++){
      c_u_str_copy_n(
          components[i].buf,
          components[i].len,
          new_buf_p + n*i,
          buf->cap - buf->len);

      buf->len += components[i].len + n*i;
      new_buf_p += components[i].len + n*i;

      buf->avl = buf->cap - buf->len;
    }
    free(buf->buf);
    buf->buf = new_buf;

  } else {
    for (size_t i = 1; i <= buf->len - index; i++){
      buf->buf[buf->len - i + n] = buf->buf[buf->len - i];
    }

    buf->len += n;
    buf->avl = buf->cap - buf->len;
  }
  return 0;
}


/*
 * Inserts `src` into a CmonBuffer `buf` at the given zero-based byte offset `index`.
 *
 * Return values:
 *   1   The insertion would exceed the current capacity; the caller must
 *       allocate/grow the buffer and retry.
 *   0   The insertion succeeded and the buffer still fits within its capacity.
 *  -1   `buf` or `src` is NULL.
 *  -2   `index` is out of range for the current buffer length.
 *  <0   Other error.
 *
 * Note: `index` is an offset into the current contents of `buf` (0..length).
 */
int c_u_buffer_insert_at(CmonBuffer *buf,
                              const char *src, 
                              size_t src_len, 
                              size_t index)
{

  char *new_buffer;
  char *new_buffer_p;
  size_t new_buffer_cap;

  size_t components_len = 3;

  CmonBufferView components[components_len];

  if (buf == NULL || src == NULL){
    errno = EINVAL;
    return -1;
  }
  
  if (index > buf->len){
    errno = ERANGE;
    return -2;
  }

  if (buf->len + src_len > buf->cap){
    components[0].buf = buf->buf;
    components[0].len = index;

    components[1].buf = src;
    components[1].len = src_len;

    components[2].buf = buf->buf + index;
    components[2].len = buf->len - index;

    new_buffer_cap = buf->len + src_len + C_U_BUFFER_INITIAL_EXTRA_SPACE;
    new_buffer = malloc(new_buffer_cap*sizeof(char));
    new_buffer_p = new_buffer;

    buf->cap = new_buffer_cap;
    buf->len = 0;
    
    for (size_t i = 0; i < components_len; i++){
      c_u_str_copy_n(
          components[i].buf,
          components[i].len,
          new_buffer_p,
          buf->cap - buf->len);

      buf->len += components[i].len;
      new_buffer_p += components[i].len;
    }

    free(buf->buf);
    buf->buf = new_buffer;

  } else {
    for(size_t i = 1; i <= buf->len - index; i++){
      buf->buf[buf->len - i + src_len] = buf->buf[buf->len - i];
    }

    buf->len += src_len;

    c_u_str_copy_n(
        src,
        src_len,
        buf->buf + index,
        buf->len);

  }
  return 0;
}


int c_u_buffer_copy(CmonBuffer   *buf,
                          char   *payload,
                          size_t  p_len)
{
  if (!buf || !payload)
    return -1;

  if (p_len == 0)
    return -2;


}


/*
 * Sets the logical length (`len`) of a `CmonBuffer`.
 *
 * This is useful after manually modifying the underlying buffer contents and
 * needing to update the recorded length (e.g., after inserting or removing
 * bytes directly).
 *
 * Returns the previous length on success.
 * Returns -1 if `new_len` exceeds the buffer capacity or on any other error.
 */
ssize_t c_u_buffer_set_len(CmonBuffer *buf, size_t new_len){
  ssize_t old_len;

  if (buf == NULL){
    return -1;
  }

  if (buf->cap < new_len){
    return -1;
  }

  old_len = buf->len; 

  buf->len = new_len;

  return old_len;
}


/*
 * Grows the CmonBuffer capacity to at least `amount`.
 * If `amount` is less than or equal to the current capacity,
 * the buffer is left unchanged.
 */
int c_u_buffer_set_capacity(CmonBuffer *buf, size_t min_cap)
{
  char *new_buf;

  if (!buf){
    return -1; 
  }

  if (min_cap <= c_u_buffer_get_cap(buf))
    return 0;

  new_buf = realloc(c_u_buffer_get_buf(buf), min_cap);
  if (!new_buf)
    return -2;

  free(c_u_buffer_get_buf(buf));
  c_u_buffer_get_buf(buf) = new_buf;
  c_u_buffer_get_cap(buf) = min_cap; 

  return 0;
}
