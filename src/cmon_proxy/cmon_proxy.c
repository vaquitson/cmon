#include <stdlib.h>
#include <sys/socket.h>
#include <pthread.h>
#include <string.h>

#include "cmon_proxy.h"
#include "cmon_sockets.h"
#include "logger.h"
#include "cmon_sockets.h"
#include "c_utils_str.h"
#include "cmon_http.h"

#define PROXY_PORT 7000
#define SERVER_PORT 3000

#define CMON_SSE_PAYLOAD "<script>\n  const es = new EventSource(\"/events\");\n  es.onmessage = () => location.reload();\n</script>\n"

/*
 * Sends `http_msg` from `sender` to `receiver`.
 *
 * Forwards the full HTTP message (header section and body). This call blocks as
 * needed to read the complete message from `sender` and write it to `receiver`.
 *
 * Returns the total number of bytes forwarded (headers + body) on success.
 * Returns 0 if the connection was closed by the peer, or -1 on error.
 */
int c_proxy_send_message(CmonHttpMessage *http_msg, int recever, size_t *send_size){
  static char *serch_tag = "</body>";
  size_t serch_tag_size = strnlen(serch_tag, 10);

  char buff_len = 2*(serch_tag_size-1);
  char *buff = malloc(sizeof(char)*buff_len); 
  char *inyected_body_chunk;
  size_t payload_size = strnlen(CMON_SSE_PAYLOAD, 100);

  ssize_t write_size;
  int read_size;
  int payload_inyect = 0;
  int rc;

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
  if (write_size == -1){
    *send_size = total_sended_size;
    return -1;
  }
  total_sended_size += write_size;
  
  while(total_sended_size < http_msg->total_msg_size){
    read_size = c_http_recv_body_chunk(http_msg);
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

    if (payload_inyect == 0){  

      rc = c_utils_find_pattern(serch_tag, http_msg->body_chunk, http_msg->cur_body_size);
      if (rc == -1){
        strncpy(buff + serch_tag_size, http_msg->body_chunk, serch_tag_size-1); 
        rc = c_utils_find_pattern(serch_tag, buff, buff_len);
        if (rc > 0){
          printf("hello\n");

          inyected_body_chunk = malloc(http_msg->cur_body_size + payload_size);
          strncpy(inyected_body_chunk, http_msg->body_chunk, rc-1);
          strncpy(inyected_body_chunk + rc, CMON_SSE_PAYLOAD, payload_size);
          strncpy(inyected_body_chunk + rc+1 + payload_size, http_msg->body_chunk+rc, http_msg->cur_body_size - rc);

          free(http_msg->body_chunk);
          http_msg->body_chunk = inyected_body_chunk;
          
          payload_inyect = 1;
        }
      } else {
        printf("hello\n");

        inyected_body_chunk = malloc(http_msg->cur_body_size + payload_size);
        strncpy(inyected_body_chunk, http_msg->body_chunk, rc-1);
        strncpy(inyected_body_chunk + rc, CMON_SSE_PAYLOAD, payload_size);
        strncpy(inyected_body_chunk + rc+1 + payload_size, http_msg->body_chunk + rc, http_msg->cur_body_size - rc);
        
        free(http_msg->body_chunk);
        http_msg->body_chunk = inyected_body_chunk;

        payload_inyect = 1;
      }
      strncpy(buff, http_msg->body_chunk + (http_msg->cur_body_size - serch_tag_size -1), serch_tag_size -1);
    }
    http_msg->body_chunk[http_msg->cur_body_size] = '\0';
    printf("\n%s\n", http_msg->body_chunk);
    write_size = c_http_send_body_chunk(http_msg, recever, C_HTTP_FLUSH_BODY_BUF);
    if (write_size == -1){
      return -1;
    }
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

void *c_proxy_handle_client(void *fd){
  int data_send;
  int client_fd;
  int server_fd;
  size_t read_size;

  client_fd = *(int *)fd;
  server_fd = c_sockets_get_server_connection(SERVER_PORT, NULL);

  for (;;) {
    // client
    CmonHttpMessage *req_msg = c_http_get_message(client_fd);
    if (req_msg == NULL){
      close(client_fd);
      close(server_fd);
      return NULL;
    };

    data_send = c_proxy_send_message(req_msg, server_fd, &read_size);
    if (data_send == -1){
      log_write(LOG_WARNING, "from c_handle_client_connection: something unexpected happen while sending");
      close(client_fd);
      close(server_fd);
      free(req_msg);
      return NULL;
    }

    if (data_send == C_HTTP_CONNECTION_CLOSED){
      close(client_fd);
      close(server_fd);
      free(req_msg);
      return NULL;
    }
    free(req_msg);

    // server 
    CmonHttpMessage *res_msg = c_http_get_message(server_fd);
    if (res_msg->headders_size == 0){
      close(client_fd);
      close(server_fd);
      return NULL;
    }

    data_send = c_proxy_send_message(res_msg, client_fd, &read_size);
    if (data_send == C_HTTP_CONNECTION_CLOSED){
      close(client_fd);
      close(server_fd);
      free(res_msg);
      return NULL;
    }
    if (data_send == -1){
      close(client_fd);
      close(server_fd);
      free(res_msg);
      return NULL;
    } 
    free(res_msg); 
  }
}

int c_proxy_start(void){
  int rc;
  int listening_fd; 
  pthread_t thread_id;
  
  socklen_t client_addr_len;
  struct sockaddr_in client_addr;

  listening_fd = c_sockets_get_listening_socket(PROXY_PORT);
  if (listening_fd < 0){
    log_write(LOG_ERROR, "from c_proxy_start: an unexpected error happen geting the listening fd");
    return -1;
  }
  
  for (;;){
    rc = accept(listening_fd, (struct sockaddr *)&client_addr, &client_addr_len);
    if (rc == -1){
      log_write(LOG_ERROR, "from c_proxy_start: an erro ocurre while accepting a connection");
    } else {
      pthread_create(&thread_id, NULL, c_proxy_handle_client, &rc);
    }
  }
}
