#pragma once

#include <stdlib.h>
#include "c_utils_buffer.h"

#define HTTP_MAX_HEADDER_SIZE 8192
#define C_HTTP_MAX_BODY_CHUNK 8192

#define C_HTTP_FLUSH_BODY_BUF 0
#define C_HTTP_NO_FLUSH_BODY_BUF 1

#define C_HTTP_CONNECTION_CLOSED -2
#define C_HTTP_BODY_CHUNK_BUFFER_FULL -3
#define C_HTTP_REQUEST_FULLY_RECEV -4

#define C_HTTP_COULD_NOT_ALLOC_MEMORY -2
#define C_HTTP_BAD_MESSAGE -3


enum CmonHttpErrCodes {
  C_HTTP_SUCESS,
  C_HTTP_NULL_ARG,
  C_HTTP_HEADER_PARSING_ERR,
  C_HTTP_RECV_ERR,
  C_HTTP_BUFFER_ERR,
  C_HTTP_SOCKET_FD_ERR,
  C_HTTP_SOCKET_CLOSE,
  C_HTTP_KEY_NOT_FOUND,
};

/* structure representing a resisable chunk of
 * http message body 
 * this chunk is compatible with casting with 
 * CmonString
*/
typedef struct {
  size_t size;
  char *buf;
  size_t length;
} CmonHttpChunk;

/* structure representing a resisable headder section of
 * http message 
 * this chunk is compatible with casting with 
 * CmonString
*/
typedef struct {
  size_t size;
  char *buf;
  size_t length;
} CmonHttpHeadder;

typedef struct {
  char headders_buff[HTTP_MAX_HEADDER_SIZE];
  char *body_chunk;

  int fd;

  ssize_t headders_size;
  ssize_t cur_body_size;
  
  size_t content_length;

  size_t total_msg_size; 
  size_t remaining_data;

} CmonHttpMessage;


ssize_t _http_get_content_length_headder(const char *buff, const size_t length);

CmonHttpMessage *c_http_get_message(int fd);

void c_http_free_message(CmonHttpMessage *msg);


ssize_t c_http_recv_header(int fd,
                           char *buff, size_t buff_len, 
                           ssize_t *read_len);

int c_http_c_buf_recv_header(int fd, CmonBuffer *buf);


int c_http_recv_body_chunk(CmonHttpMessage *msg);

ssize_t c_http_send_headers(CmonHttpMessage *msg, int fd);
ssize_t c_http_send_body_chunk(CmonHttpMessage *msg, int recever, int op);

int c_http_get_header_field(
    const char *header, size_t header_size, 
    const char *key   , size_t key_size,
          char *buf   , size_t buf_size);

int c_http_buf_get_header_field(CmonBuffer *header, char *key, size_t key_size, CmonBuffer *buf);

int c_http_set_headder(
    CmonBuffer *header,
    const char *key    , size_t key_size,
    const char *content, size_t content_size);


