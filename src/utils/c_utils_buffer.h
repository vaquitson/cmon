#pragma once

#include <stdlib.h>

#define C_U_BUFFER_INITIAL_EXTRA_SPACE 20
#define C_U_BUFFER_VIEW_END -1

typedef struct {
  size_t cap;
  size_t len;
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


CmonBuffer *c_u_buffer_empty(size_t cap);

CmonBuffer *c_u_buffer_new(char *src, size_t len);

void c_u_buffer_free(CmonBuffer *buf);

CmonBufferView *c_u_view_buffer_new(const char *buf, const size_t size, size_t start, ssize_t len);

int c_u_buffer_insert_at(CmonBuffer *buf,
                                 char *payload, 
                                 size_t p_len, 
                                 size_t index);


void c_u_buffer_print(CmonBuffer *buf);
