#include <bits/posix_opt.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>

#include "cmon_http.h"
#include "s_client_connection.h"
#include "cmon_sockets.h"
#include "logger.h"

#define READ_BUFF_LEN 16384
#define SERVER_PORT 3000

int listener_fd;
uint16_t listener_port;

void * cc_handle_client_connection(void *fd){
  int data_send;
  size_t read_size;

  int client_fd = *(int *)fd;
  int server_fd = c_sockets_get_server_connection(SERVER_PORT, NULL);
  
  for (;;) {
    // client
    CmonHttpMessage *req_msg = c_http_get_message(client_fd);
    if (req_msg == NULL){
      close(client_fd);
      close(server_fd);
      return NULL;
    };

    data_send = c_http_send_message(req_msg, server_fd, &read_size);
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
    
    data_send = c_http_send_message(res_msg, client_fd, &read_size);
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
