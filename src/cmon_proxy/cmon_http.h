#pragma once

#include <stdlib.h>

#define HTTP_MAX_HEADDER_SIZE 8192
#define C_HTTP_MAX_BODY_CHUNK 8192

#define C_HTTP_FLUSH_BODY_BUF 0
#define C_HTTP_NO_FLUSH_BODY_BUF 1

#define C_HTTP_CONNECTION_CLOSED -2
#define C_HTTP_BODY_CHUNK_BUFFER_FULL -3
#define C_HTTP_REQUEST_FULLY_RECEV -4

#define C_HTTP_COULD_NOT_ALLOC_MEMORY -2
#define C_HTTP_BAD_MESSAGE -3

typedef struct {
  char headders_buff[HTTP_MAX_HEADDER_SIZE];
  char body_chunk[C_HTTP_MAX_BODY_CHUNK];

  int fd;

  ssize_t headders_size;
  ssize_t cur_body_size;
  
  ssize_t content_length;
  size_t total_msg_size;
  
  size_t remaining_data;

} CmonHttpMessage;

ssize_t _http_get_headders(int fd, char *buff, size_t buff_len, ssize_t *read_len);
CmonHttpMessage *c_http_get_message(int fd);
ssize_t _http_get_content_length_headder(const char *buff, const size_t length);
int c_http_send_message(CmonHttpMessage *http_msg, int recever, size_t *send_size);


