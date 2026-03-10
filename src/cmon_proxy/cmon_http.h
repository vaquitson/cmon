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

ssize_t _http_get_headders(int fd, char *buff, size_t buff_len, ssize_t *read_len);
ssize_t _http_get_content_length_headder(const char *buff, const size_t length);

CmonHttpMessage *c_http_get_message(int fd);
ssize_t c_http_send_headers(CmonHttpMessage *msg, int fd);
int c_http_recv_body_chunk(CmonHttpMessage *msg);
ssize_t c_http_send_body_chunk(CmonHttpMessage *msg, int recever, int op);
void c_http_free_message(CmonHttpMessage *msg);
