#include <bits/posix_opt.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>

#include "cmon_http.h"
#include "s_client_connection.h"
#include "cmon_sockets.h"

#define READ_BUFF_LEN 16384
#define SERVER_PORT 3000

int listener_fd;
uint16_t listener_port;

void * cc_handle_client_connection(void *fd){
  ssize_t data_send;

  int client_fd = *(int *)fd;
  int server_fd = c_sockets_get_server_connection(SERVER_PORT, NULL);
  
  for (;;) {
    // client
    CmonHttpMessage *req_msg = c_http_get_message(client_fd);
    if (req_msg->headders_size == 0){
      return NULL;
    }

    data_send = c_http_send_message(client_fd, server_fd, req_msg);
    if (data_send == 0){
      return NULL;
    }

    // server
    CmonHttpMessage *res_msg = c_http_get_message(server_fd);
    if (res_msg->headders_size == 0){
      return NULL;
    }
    
    data_send = c_http_send_message(server_fd, client_fd, res_msg);
    if (data_send == 0){
      return NULL;
    }
  }
}
