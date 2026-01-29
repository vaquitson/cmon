#pragma once

#include <stdlib.h>

#define HTTP_MAX_HEADDER_SIZE 8192

typedef struct {
  char headders_buff[HTTP_MAX_HEADDER_SIZE];
  char body_buff[HTTP_MAX_HEADDER_SIZE];

  ssize_t headders_size;
  ssize_t cur_body_size;
} CmonHttpMessage;

ssize_t _http_get_headders(int fd, char *buff, size_t buff_len, ssize_t *read_len);
CmonHttpMessage *http_get_message(int fd);
ssize_t _http_get_content_length_headder(const char *buff, const size_t length);
ssize_t http_send_http_message(int sender, int recever, CmonHttpMessage *http_msg);



