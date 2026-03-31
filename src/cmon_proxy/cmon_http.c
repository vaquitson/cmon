#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <string.h>

#include "logger.h"
#include "cmon_http.h"
#include "c_utils_str.h"
#include "c_utils_buffer.h"
#include "cmon_sockets.h"

#define MAX_ITERATIONS 20
#define MAX_LENGTH_OF_CONTEMT_LENGTH_CHARS 9
#define MAX_LENGTH_OF_PARSEABLE_STRING 9

#define KEY_NOT_FOUND -3

#define CRLF "\r\n"
#define CRLF_S 2

#define C_HTTP_HEADER_DELIM "\r\n\r\n"
#define C_HTTP_HEADER_DELIM_SIZE 4

#define _c_http_try_find_end_of_header(buf, buff_len) c_u_str_find_pattern(C_HTTP_HEADER_DELIM, C_HTTP_HEADER_DELIM_SIZE, buf, buff_len)

struct CmonUtilRange{
  size_t start;
  size_t end;
};

/*
 * Parses the string `str` as a non-negative decimal integer.
 *
 * Returns the parsed value on success, or -1 on error.
 * The input must be at most MAX_LENGTH_OF_PARSEABLE_STRING characters long;
 * otherwise, the function returns -1.
 */
ssize_t _parse_str_to_int(char *str, size_t size){
  if (size > MAX_LENGTH_OF_PARSEABLE_STRING){
    return -1;
  }
  size_t value = 0;
  char c;
  for (size_t i=0; i < size; i++){
    c = str[i];
    if (c >= '0' && c <= '9'){
      value *= 10;
      value += (ssize_t)(c - '0');

    } else if (c != ' ') {
      log_write(LOG_ERROR, 
          "from _parse_str_to_int: the string contains non numerical characters");
      return -1; 
    } 
  }
  return value;
}

/*
 * Reads from `fd` until a complete HTTP header block is available (terminated
 * by the "\r\n\r\n" sequence) and stores the bytes into `buff`.
 *
 * Returns the length (in bytes) of the HTTP headers, including the terminating
 * "\r\n\r\n" sequence, or -1 on failure.
 *
 * On return, `*read_len` contains the total number of bytes read from `fd` and
 * written into `buff`. This may be larger than the header length if the read
 * operation consumed bytes past the end of the headers (i.e., the beginning of
 * the message body). Those extra bytes are left in `buff` immediately after the
 * headers so the caller can reuse them and avoid losing data.
 *
 * if the connection is closed by reading 0 bytes, the function returns 0;
 *
 * On faild -1 is return
*/
ssize_t c_http_recv_header(int fd, char *buff, size_t buff_len, ssize_t *read_len){
  ssize_t read_size;
  ssize_t p_offset;
  ssize_t tot_size = 0;

  for (;;){
    if (buff_len <= 0){
      log_write(LOG_ERROR,
          "from _get_http_headders: the length of the hedders is to large for the buffer of size %ld",
          buff_len);
      return -1;
    }

    read_size = read(fd, buff+tot_size, buff_len);
    if (read_size < 0) {
      log_write(LOG_ERROR, 
          "from _http_get_headders: read errror -> %s",
          strerror(errno));
      return -1;
    }

    if (read_size == 0){
      *read_len = tot_size;
      return 0;
    }

    tot_size += read_size;

    if ((p_offset = c_u_str_find_pattern("\r\n\r\n", strlen("\r\n\r\n"), buff, tot_size)) != -1){
      *read_len = tot_size;
      return p_offset + 4;

    } else if ((p_offset = c_u_str_find_pattern("\n\n", strlen("\n\n"), buff, tot_size)) != -1) {
      *read_len = tot_size;
      return p_offset + 4;
    }

    buff_len -= read_size;
  }
}


/*
 * Reads from `fd` until a complete HTTP header block is available (terminated
 * by the "\r\n\r\n" sequence) and stores the bytes into `buff`.
 *
 * On Succes fill the CmonBuffer with the content of the Header including the end 
 * sequence
 *
 * On error retrun a code from CmonHttpErrCodes
 *
*/
int c_http_recv_header1(int fd, CmonBuffer *buf)
{
  int rc;
  ssize_t peek_size; 

  ssize_t recv_expect_size;
  ssize_t recv_size;

  ssize_t p_offset;

  char peek_buf[HTTP_MAX_HEADDER_SIZE] = {0};

  if (!buf || c_u_buffer_get_cap(buf) == 0 || c_u_buffer_get_len(buf) != 0)
    return C_HTTP_BUFFER_ERR;

  if (fd < 0)
    return C_HTTP_SOCKET_FD_ERR;

  peek_size = c_sockets_peek(fd, peek_buf, HTTP_MAX_HEADDER_SIZE);
  if (peek_size == -1)
    return C_HTTP_RECV_ERR;
  if (peek_size == 0)
    return C_HTTP_SOCKET_CLOSE;

  p_offset = _c_http_try_find_end_of_header(peek_buf, peek_size);
  if (p_offset == -1)
    return C_HTTP_HEADER_PARSING_ERR;

  recv_expect_size = p_offset + C_HTTP_HEADER_DELIM_SIZE;

  if (c_u_buffer_get_cap(buf) < (size_t)recv_expect_size){
    if (c_u_buffer_set_capacity(buf, recv_expect_size) != 0)
      return C_HTTP_BUFFER_ERR;
  }

  recv_size = recv(fd, c_u_buffer_get_buf(buf), recv_expect_size, 0);
  if (recv_size == -1 || recv_size != recv_expect_size)
    return C_HTTP_RECV_ERR;
  if (recv_size == 0)
    return C_HTTP_SOCKET_CLOSE;

  if (c_u_buffer_set_len(buf, recv_size) == -1)
    return C_HTTP_BUFFER_ERR;

  return C_HTTP_SUCESS;
}


/* this function send the header segment of the 
 * CmonHttpMessage to the provided fd.
 *
 * On succes return the number of bytes writed
 * on error return -1
*/ 
ssize_t c_http_send_headers(CmonHttpMessage *msg, int fd){
  if (msg == NULL){
    return -1;
  }

  ssize_t write_size; 

  write_size = write(fd, msg->headders_buff, msg->headders_size);
  if (write_size < 0){
    log_write(LOG_ERROR, "from c_http_send_headers: an error ocurre while reading -> %s", strerror(errno)); 
    return -1;
  }
  
  // This might be recoverable
  if (write_size != msg->headders_size){
    log_write(LOG_ERROR, "from c_http_send_headers: coulden't send the full headder");
    return -1;
  }

  return write_size;
}


/*
 * This function gets a buffer with an http headders of a (req,res) and
 * return the value of the Content-Length headder.
 * If the headder dosent exist or its value is 0, 0 is retrun.
 *
 * on error return -1
 */
ssize_t _http_get_content_length_headder(const char *buff, const size_t length){
  ssize_t heaader_index;
  size_t number_buf_size = 0;
  ssize_t parse_val;

  int iterations = 0;
  char number_buf[MAX_LENGTH_OF_CONTEMT_LENGTH_CHARS] = {'\0'};
  const char *p_c;

  heaader_index = c_u_str_find_pattern("Content-Length:", strlen("Content-Length:"), buff, length);
  if (heaader_index == -1){
    return 0;
  } 

  p_c = buff + heaader_index + 14;

  while (*p_c != '\n' && *p_c != '\r'){
    if (iterations > MAX_ITERATIONS){
      log_write(LOG_ERROR,
          "from _http_get_content_length_headder: bad request");
      return -1;
    }

    if (number_buf_size > MAX_LENGTH_OF_CONTEMT_LENGTH_CHARS){
      log_write(LOG_ERROR,
          "from _http_get_content_length_headder: the content length is to long");
      return -1;
    }

    if (*p_c >= '0' && *p_c <= '9'){
      number_buf[number_buf_size] = *p_c;
      number_buf_size++;
    } else {
      iterations++;
    }
    p_c++;
  }

  if (number_buf_size == 0){
    return 0;
  } 
  
  parse_val = _parse_str_to_int(number_buf, number_buf_size);
  if (parse_val < 0){
    log_write(LOG_ERROR,
        "from _http_get_content_length_headder: parse error");
    return -1;
  }

  return parse_val;
}

int _c_http_get_key_value_range(
    const char *header, size_t header_size,
    const char *key   , size_t key_size,
    struct CmonUtilRange *range)
{     
  size_t doted_key_size = key_size + 1;
  char doted_key[doted_key_size];

  ssize_t offset;
  ssize_t start_index;
  ssize_t header_value_size;

  if (header == NULL || key == NULL){
    return -1;
  }

  if (header_size == 0 || key_size == 0){
    return -2;
  }
  
  memcpy(doted_key, key, key_size); 
  doted_key[key_size] = ':';

  offset = c_u_str_find_pattern(doted_key,
                                doted_key_size,
                                header,
                                header_size);


  if (offset == -1){
    return KEY_NOT_FOUND;
  }

  start_index = offset + key_size + 1; 
  header_value_size = c_u_str_find_pattern(CRLF, 
                                   CRLF_S,
                                   header + start_index,
                                   header_size - start_index);

  if (header_value_size == -1){
    return -4;
  }

  range->start = start_index;
  range->end = header_value_size;

  return 0;
}

/*
 * Copies the value of the header field `key` from `header` into `buf`.
 *
 * Searches within the `header` buffer (length `header_size`) for the field name
 * `key` followed by ':' (i.e., "key:"). If found, copies the bytes after the
 * colon up to the next CRLF into `buf`.
 *
 * The copied value is NOT NUL-terminated.
 *
 * Returns the number of bytes copied on success.
 * returns 0 if the key dosent exist
 * Returns -1 on NULL arguments, -2 on zero sizes, -3 if `key:`/CRLF is not found,
 * and -4 if the value does not fit in `buf` or is too large to return as `int`.
 */
int c_http_get_header(
    const char *header, size_t header_size, 
    const char *key   , size_t key_size,
          char *buf   , size_t buf_size){
  

  size_t doted_key_size = key_size + 1;
  char doted_key[doted_key_size];

  ssize_t offset;
  ssize_t start_index;
  ssize_t header_value_size;

  if (header == NULL || buf == NULL ||  key == NULL){
    return -1;
  }

  if (header_size == 0 || buf_size == 0 || key_size == 0){
    return -2;
  }
  
  memcpy(doted_key, key, key_size); 
  doted_key[key_size] = ':';

  offset = c_u_str_find_pattern(doted_key,
                                doted_key_size,
                                header,
                                header_size);


  if (offset == -1){
    return 0;
  }

  start_index = offset + key_size + 1; 
  header_value_size = c_u_str_find_pattern(CRLF, 
                                   CRLF_S,
                                   header + start_index,
                                   header_size - start_index);

  if (header_value_size == -1){
    return -3;
  }

  if (header_value_size > INT_MAX){
    return -4;
  }

  if (buf_size < (size_t)header_value_size){
    return -4;
  }

  memcpy(buf, header + start_index, header_value_size);
  
  return (int)header_value_size; 
}



/*
 * This function set the value of the "headder" parameter to "val"
 * the buff parameter cotains the buffer with the headder part of the 
 * http message wich have size size
*/
int c_http_set_headder(CmonBuffer *header,
    const char *key    , size_t key_size,
    const char *content, size_t content_size){
  
  struct CmonUtilRange range;
  int rc;
  size_t cur_header_value_len;

  rc = _c_http_get_key_value_range(
      header->buf, header->len,
      key, key_size,
      &range);

  if (rc == 0){
    cur_header_value_len = range.end - range.start - 1;
    if (cur_header_value_len < content_size) { 
    }

  } 

  return 0;
}


/*
 * Blocks until an HTTP request is received.
 *
 * Reads the complete HTTP header section (up to "\r\n\r\n") and returns a
 * heap-allocated CmonHttpMessage representing the request.
 *
 * This function may read past the end of the header section. Any excess bytes
 * (i.e., bytes already read beyond "\r\n\r\n") are stored in `msg->body_chunk`,
 * and `msg->cur_body_size` is set to the number of excess bytes. If no excess
 * bytes are read, `msg->cur_body_size` is set to 0.
 *
 * Note: This function does not guarantee that the full message body is read.
 * It only reads the header section and stores any body bytes that happened to
 * arrive in the same read.
 *
 * If the peer closes the connection before a full header section is read, the
 * returned message will have `msg->header_size == 0`.
 *
 * The caller owns the returned message and must free it when done.
 *
 */
CmonHttpMessage *c_http_get_message(int fd){
  ssize_t total_read_size;
  ssize_t headders_size;
  ssize_t content_length;

  if (fd < 0){
    log_write(LOG_ERROR, "from c_http_get_message: the fd is less than 0");
    return NULL;
  }

  CmonHttpMessage *http_message = malloc(sizeof(CmonHttpMessage));
  if (http_message == NULL){
    log_write(LOG_ERROR, 
        "from c_http_get_message: coudl not allocate memory for the http_message -> %s",
        strerror(errno));
    return NULL;
  }

  http_message->body_chunk = malloc(C_HTTP_MAX_BODY_CHUNK);
  if (http_message->body_chunk == NULL){
    free(http_message);
    return NULL;
  }

  http_message->cur_body_size = 0;
  http_message->headders_size = 0;
  http_message->remaining_data = 0;
  
  headders_size = c_http_recv_header(fd, 
      http_message->headders_buff, 
      HTTP_MAX_HEADDER_SIZE, 
      &total_read_size);

  if (headders_size < 0){
    log_write(LOG_WARNING, 
        "from c_http_get_message: bad request, heeader size is %ld",
        headders_size);
    free(http_message);
    return NULL;
  }
  
  if (headders_size == 0){
    free(http_message);
    return NULL;
  }

  http_message->headders_size = headders_size;

  for (ssize_t i = 0; i < total_read_size - headders_size; i++){
    http_message->body_chunk[i] = http_message->headders_buff[headders_size+i];
  }

  content_length = _http_get_content_length_headder(
      http_message->headders_buff, 
      http_message->headders_size);

  if (content_length < 0){
    log_write(LOG_ERROR, "c_http_get_message: something go wrong getting the content length header");
    free(http_message);
    return NULL;
  }

  http_message->total_msg_size = headders_size + content_length;
  http_message->content_length = content_length;
  http_message->cur_body_size = total_read_size - headders_size;
  http_message->remaining_data = http_message->total_msg_size - total_read_size; 
  http_message->fd = fd;

  return http_message;
}

/*
 * Sends the bytes currently stored in `msg->body_chunk` to the receiver file
 * descriptor.
 *
 * `op` is a bitmask of optional flags defined in cmon_http.h.
 *
 * If `op` is 0 (default), the buffer is considered consumed after sending:
 * `msg->cur_body_size` is reset to 0.
 *
 * Returns the number of bytes written on success (it should match the value of
 * `msg->cur_body_size` before any reset). On error, logs the failure and
 * returns -1.
 */
ssize_t c_http_send_body_chunk(CmonHttpMessage *msg, int recever, int op) {
  if (msg == NULL){
    log_write(LOG_ERROR, "from c_http_send_body: the CmonHttpMessage is NULL");
    return -1;
  }

  if (recever < 0){
    log_write(LOG_ERROR, "from c_http_send_body: the recever fd is less than 0");
    return -1;
  }

  ssize_t write_size;

  write_size = write(recever,
      msg->body_chunk, 
      msg->cur_body_size);

  if (write_size < 0){
    log_write(LOG_ERROR, "from c_http_send_body: error while writing -> %s",
        strerror(errno));
    return -1; 
  }
  
  // This might be recoverable
  if (write_size != msg->cur_body_size){
    log_write(LOG_ERROR, "from c_http_send_body: could not send the full body buffer");
    return -1;
  }

  if (op == C_HTTP_FLUSH_BODY_BUF){
    msg->cur_body_size = 0;
  }

  return write_size;
}


/*
 * Blocks until a new body chunk is available and stores it in `msg->body_chunk`.
 *
 * Returns the number of bytes read on success.
 * Returns C_HTTP_CONNECTION_CLOSED if the HTTP connection was closed by the peer.
 * Returns C_HTTP_BODY_CHUNK_BUFFER_FULL if `msg->body_chunk` is full and must be
 * consumed before reading more.
 *
 * On error, logs the failure and returns -1.
 */
int c_http_recv_body_chunk(CmonHttpMessage *msg){
  ssize_t read_size;

  if (msg == NULL){
    log_write(LOG_ERROR, "from c_http_recev_http_body_chunk: the msg is NULL");
    return -1;
  }

  if (msg->fd < 0){
    log_write(LOG_WARNING, "from c_http_recev_http_body_chunk: the connection is closed");
    return C_HTTP_CONNECTION_CLOSED; 
  }

  if (msg->cur_body_size >= C_HTTP_MAX_BODY_CHUNK){
    log_write(LOG_WARNING, "from c_http_recev_http_body_chunk: trying to recev a new body chunk with a full buffer");
    return C_HTTP_BODY_CHUNK_BUFFER_FULL;
  }

  if (msg->remaining_data <= 0){
    log_write(LOG_WARNING, "from c_http_recev_http_body_chunk: trying to read a message that has been already fully recev");
    return C_HTTP_REQUEST_FULLY_RECEV;
  }
 
  read_size = read(
      msg->fd,
      msg->body_chunk + msg->cur_body_size,
      C_HTTP_MAX_BODY_CHUNK - msg->cur_body_size
      );
 
  // this might be recoverable
  if (read_size < 0){
    log_write(LOG_ERROR, "from c_http_recev_http_body_chunk: an error ocurre reading -> ", strerror(errno));
    return -1;
  }

  msg->remaining_data -= read_size;
  msg->cur_body_size += read_size;

  return read_size;
}


/*
 * Frees a CmonHttpMessage.
 *
 * If `msg` is NULL, this function does nothing.
 */
void c_http_free_message(CmonHttpMessage *msg){
  if (msg == NULL){
    return;
  }

  if (msg->body_chunk != NULL){
    free(msg->body_chunk);
  }
  free(msg);
}

