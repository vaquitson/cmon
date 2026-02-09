#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <string.h>

#include "logger.h"
#include "cmon_http.h"

#define MAX_ITERATIONS 20
#define MAX_LENGTH_OF_CONTEMT_LENGTH_CHARS 9
#define MAX_LENGTH_OF_PARSEABLE_STRING 9


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
 * Searches for the pattern `p` in the text `t`.
 *
 * Returns the zero-based index (as `ssize_t`) of the first occurrence of `p` in `t`,
 * or -1 if no match is found.
*/
ssize_t _find_patern(const char *p, const char *t, const size_t t_len){
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
ssize_t _http_get_headders(int fd, char *buff, size_t buff_len, ssize_t *read_len){
  ssize_t read_size;
  ssize_t p_offset;
  ssize_t tot_size = 0;

  for (;;){
    if (buff_len <= 0){
      log_write(LOG_ERROR,
          "from _get_http_headders: the length of the hedders is to large");
      return -1;
    }

    read_size = read(fd, buff, buff_len);
    if (read_size < 0) {
      log_write(LOG_ERROR, 
          "from _http_get_headders: read errror -> %s",
          strerror(errno));
      return -1;
    }

    if (read_size == 0){
      log_write(LOG_WARNING,
          "from _http_get_headders: the connection was closed before the end of the headers");
      *read_len = tot_size;
      return 0;
    }

    tot_size += read_size;  
    
    if ((p_offset = _find_patern("\r\n\r\n", buff, tot_size)) != -1){
      *read_len = tot_size;
      return p_offset + 4;

    } else if ((p_offset = _find_patern("\n\n", buff, tot_size)) != -1) {
      *read_len = tot_size;
      return p_offset + 4;
    }

    buff_len -= read_size;
    buff += read_size;
  }
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

  heaader_index = _find_patern("Content-Length:", buff, length);
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
 */

CmonHttpMessage *c_http_get_message(int fd){
  ssize_t total_read_size;
  ssize_t headders_size;
  ssize_t content_length;

  if (fd < 0){
    log_write(LOG_ERROR, "from http_get_message: the fd is less than 0");
    return NULL;
  }

  CmonHttpMessage *http_message = malloc(sizeof(CmonHttpMessage));
  if (http_message == NULL){
    log_write(LOG_ERROR, 
        "from c_http_get_message: coudl not allocate memory for the http_message -> %s",
        strerror(errno));
    return NULL;
  }

  http_message->cur_body_size = 0;
  http_message->headders_size = 0;
  http_message->remaining_data = 0;
  
  headders_size = _http_get_headders(fd, 
      http_message->headders_buff, 
      HTTP_MAX_HEADDER_SIZE, 
      &total_read_size);

  if (headders_size <= 0){
    log_write(LOG_WARNING, 
        "from http_get_message: bad request, heeader size is %ld",
        headders_size);
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
    log_write(LOG_ERROR, "from http_get_message: something go wrong getting the content length header");
    free(http_message);
    return NULL;
  }

  http_message->total_msg_size = headders_size + content_length;
  http_message->content_length = content_length;
  http_message->cur_body_size = total_read_size - headders_size;
  http_message->fd = fd;

  return http_message;
}



/*
 * Sends `http_msg` from `sender` to `receiver`.
 *
 * Returns the total number of bytes sent (headers + body).
 * Returns 0 if the connection was closed by the peer, or -1 on error.
 */
ssize_t c_http_send_message(int sender, int recever, CmonHttpMessage *http_msg){
  ssize_t content_length;
  ssize_t write_size;
  ssize_t read_size;

  ssize_t total_sended_size = 0;

  if (http_msg == NULL){
    log_write(LOG_ERROR, "from c_http_send_message: the http message is null");
    return -1;
  }

  if (sender < 0 || recever < 0){
    log_write(LOG_ERROR, "from c_http_send_message: the fd are not valid");
    return -1;
  }

  if (http_msg->headders_size == 0){
    return 0;
  }

  content_length = _http_get_content_length_headder(http_msg->headders_buff, http_msg->headders_size); 
  if (content_length < 0){
    log_write(LOG_ERROR, "from c_http_send_message: the content length is incorrect");
    return -1;
  }

  write_size = write(recever, http_msg->headders_buff, http_msg->headders_size);
  if (write_size < 0){
    log_write(LOG_ERROR, 
        "from c_http_send_message: error while trying to send headders to recever -> %s", strerror(errno));
    return -1;
  }

  if (write_size != http_msg->headders_size){
    log_write(LOG_ERROR, 
        "from http_send_http_message: error while trying to send headders to recever, the length dosent match, expect to send %ld send %ld",
        http_msg->headders_size,
        write_size);
    return -1;
  }
  
  total_sended_size += write_size;

  if (http_msg->cur_body_size == content_length){
    write_size = write(recever, http_msg->body_chunk, http_msg->cur_body_size);

    if (write_size != http_msg->cur_body_size){
      log_write(LOG_ERROR, 
          "from http_send_http_message: write less data than expected. Expect %ld send %ld",
          http_msg->cur_body_size,
          write_size);
      return -1;
    }
    total_sended_size += write_size;
  }

  while(total_sended_size < http_msg->headders_size + content_length){
    read_size = read(sender, 
        http_msg->body_chunk + http_msg->cur_body_size, 
        HTTP_MAX_HEADDER_SIZE - http_msg->cur_body_size);

    if (read_size < 0){
      log_write(LOG_ERROR, 
          "from http_send_http_message: a problem ocurre while reading the sender -> %s", 
          strerror(errno));
      return -1;
    }
    
    if (read_size == 0){
      log_write(LOG_WARNING, "from http_send_http_message: the connection was closed before all data is sent. Expect to send %ld send %ld",
          http_msg->headders_size + content_length,
          total_sended_size);
      break;
    }


    http_msg->cur_body_size += read_size; 
    write_size = write(recever, http_msg->body_chunk, http_msg->cur_body_size);

    if (write_size != http_msg->cur_body_size){
      log_write(LOG_ERROR, 
          "from http_send_http_message: write less data than expected. Expect %ld send %ld",
          http_msg->cur_body_size,
          write_size);
      return -1;
    }

    
    http_msg->cur_body_size = 0;
    total_sended_size += write_size;

  }

  if (total_sended_size == http_msg->headders_size + content_length){
    return total_sended_size;

  } else {
    log_write(LOG_ERROR, "from http_send_http_message: expect to send %ld but send %ld",
        http_msg->headders_size + content_length,
        total_sended_size);
    return -1;
  }
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
ssize_t c_http_send_body(CmonHttpMessage *msg, int recever, int op) {
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
int c_http_recev_http_body_chunk(CmonHttpMessage *msg){
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
 * Sends `http_msg` from `sender` to `receiver`.
 *
 * Forwards the full HTTP message (header section and body). This call blocks as
 * needed to read the complete message from `sender` and write it to `receiver`.
 *
 * Returns the total number of bytes forwarded (headers + body) on success.
 * Returns 0 if the connection was closed by the peer, or -1 on error.
 */
int c_http_send_message1(CmonHttpMessage *http_msg, int recever, size_t *send_size){
  ssize_t write_size;
  int read_size;

  size_t total_sended_size = 0;

  if (http_msg == NULL){
    log_write(LOG_ERROR, "from c_http_send_message: the http message is null");
    *send_size = total_sended_size;
    return -1;
  }

  if (recever < 0){
    log_write(LOG_ERROR, "from c_http_send_message: the fd are not valid");
    *send_size = total_sended_size;
    return -1;
  }

  write_size = c_http_send_headers(http_msg, recever);
  if (write_size < 0){
    *send_size = total_sended_size;
    return -1;
  } 
  total_sended_size += write_size;
  
  while(total_sended_size < http_msg->total_msg_size){
    read_size = c_http_recev_http_body_chunk(http_msg);
    if (read_size == -1){
      log_write(LOG_ERROR ,"from c_http_send_message: something went wrong while reding");
      *send_size = total_sended_size;
      return -1;
    }

    if (read_size == C_HTTP_CONNECTION_CLOSED){
      log_write(LOG_WARNING, "from c_http_send_message: the connection was closed unexpectedly");
      *send_size = total_sended_size;
      return C_HTTP_CONNECTION_CLOSED;
    }

    write_size = c_http_send_body(http_msg, recever, 0);
    total_sended_size += write_size;
  }

  if (total_sended_size == http_msg->total_msg_size){
    *send_size = total_sended_size;
    return total_sended_size;

  } else {
    log_write(LOG_ERROR, "from c_http_send_message: expect to send %ld but send %ld",
        http_msg->total_msg_size,
        total_sended_size);
    return -1;
  }
}
