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
      log_write(LOG_ERROR, "from cc_handle_client_connection: the req msg has problems");
      return NULL;
    }

    data_send = c_http_send_message1(req_msg, server_fd, &read_size);
    if (data_send == 0){
      return NULL;
    }

    // server
    CmonHttpMessage *res_msg = c_http_get_message(server_fd);
    if (res_msg->headders_size == 0){
      return NULL;
    }
    
    data_send = c_http_send_message1(res_msg, client_fd, &read_size);
    if (data_send == 0){
      return NULL;
    }
  }
}
