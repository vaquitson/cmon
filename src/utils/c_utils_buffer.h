#pragma once

#include <stdlib.h>

#define C_U_BUFFER_INITIAL_EXTRA_SPACE 20
#define C_U_BUFFER_VIEW_END -1

/* this struct represent a resizable buffer
 * cap is the total capacity of the buffer 
 *
 * len is the current len of the buffer or in other
 * words, the space ocupy
 *
 * avl is the ramaining sapce in the buffer 
 * 
 * buf is a pointer to the buffer memory
*/

typedef struct {
  size_t cap;
  size_t len;
  size_t avl;
  char *buf;
} CmonBuffer;


/*
 * this struct is used to give a view of 
 * a buffer (i dosent matter if it is a Cmon buffer or not)
 * 
 * You should be very carfule when iteractig with this
 * struct, because it is very easy to hold a view of a
 * memory that has been free 
*/
typedef struct {
  size_t len;
  const char *buf;
} CmonBufferView;


#define c_u_buffer_get_cap(buffer) (buffer)->cap
#define c_u_buffer_get_buf(buffer) (buffer)->buf
#define c_u_buffer_get_len(buffer) (buffer)->len

#define c_u_buffer_append(buffer, payload, payload_len) c_u_buffer_insert_at((buffer), (payload), (payload_len), c_u_buffer_get_len((buffer)))

CmonBuffer *c_u_buffer_empty(size_t cap);

CmonBuffer *c_u_buffer_new(const char *src, size_t len);

void c_u_buffer_free(CmonBuffer *buf);

CmonBufferView *c_u_view_buffer_new(const char *buf, const size_t size, size_t start, ssize_t len);

int c_u_buffer_insert_at(CmonBuffer *buf,
                             const char *payload, 
                             size_t  p_len, 
                             size_t  index);

int c_u_buffer_copy(CmonBuffer   *buf,
                          char   *payload,
                          size_t  p_len);

int c_u_buffer_shift_from(CmonBuffer *buf, size_t index, size_t n);

void c_u_buffer_print(CmonBuffer *buf);

ssize_t c_u_buffer_set_len(CmonBuffer *buf, size_t new_len);

int c_u_buffer_set_capacity(CmonBuffer *buf, size_t min_cap);
