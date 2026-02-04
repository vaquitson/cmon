#include <bits/posix_opt.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>

#include "logger.h"
#include "cmon_http.h"
#include "s_client_connection.h"

#define READ_BUFF_LEN 16384
#define SERVER_PORT 3000

int listener_fd;
uint16_t listener_port;


/*
 * Attempts to open a listening socket on the specified port.
 *
 * On success, returns the file descriptor of the listening socket.
 * On failure, returns a negative value.
 */
int _get_listening_socket(uint16_t port){
  static struct sockaddr_in listener_addr;
  int sd;
  int rc;
  
  sd = socket(AF_INET, SOCK_STREAM, 0);
  if (sd < 0){
    log_write(LOG_ERROR,
        "from _get_listening_socket: could not open a socket -> %s", 
        strerror(errno));
    return -1;
  }
  
  bzero(&listener_addr, sizeof(listener_addr));
  listener_addr.sin_family = AF_INET;
  listener_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  listener_addr.sin_port = htons(port);

  rc = bind(sd, 
      (struct sockaddr *) &listener_addr, 
      sizeof(listener_addr));

  if (rc != 0 ){ 
    log_write(LOG_ERROR, 
        "from _get_listening_socket: could not bind the sockaddr to the server -> %s",
        strerror(errno));
    return -1;
  }

  if (listen(sd, 0) < 0){
    log_write(LOG_ERROR, 
        "from _get_listening_socket: could not start a listening socket -> %s",
       strerror(errno));

    return -2;
  }
  
  log_write(LOG_INFO,
      "from _get_listening_socket: sokcet listening on port %ld",
      port);

  return sd;
}



/*
 * Attempts to connect to the given address, retrying indefinitely until it succeeds.
 *
 * This call blocks with no timeout until a connection is established.
 * Returns the connected socket file descriptor.
 */
int _try_connect(struct sockaddr_in *addr){
  int sock_fd;
  int rc;
  log_write(LOG_INFO,
      "from _get_client_connection: trying to connect to the server");
  for (;;){
    sock_fd = socket(AF_INET, SOCK_STREAM, 0); 
    if (sock_fd < 0){
      log_write(LOG_ERROR,
          "from _get_client_connection: could not start a socket -> %s",
          strerror(errno));
      return -1;
    }
    
    rc = connect(sock_fd, (struct sockaddr *)addr, sizeof(*addr));
    if (rc == 0){
      log_write(LOG_INFO,
          "from _try_connect: connection stablish");
      return sock_fd;
    }
    close(sock_fd);
  }
}


/*
 * Establishes a TCP connection to the given address and port and returns the
 * connected socket file descriptor.
 *
 * If `address` is NULL, the function connects to localhost.
 * This call blocks (no timeout) until the connection is established.
 *
 * Returns:
 *   - a non-negative file descriptor on success
 *   - -1 on failure
 */
int _get_server_connection(const uint16_t port, const char *addr){
  struct sockaddr_in server_addr; 
  int sock_fd;
  const char *ip_addr;

  if (addr == NULL){
    ip_addr = "127.0.0.1";
    log_write(LOG_INFO, 
        "from _get_server_connection: the ip addres is set to localhost");
  } else {
    ip_addr = addr;
  }

  bzero(&server_addr, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);
  inet_pton(AF_INET, ip_addr, &server_addr.sin_addr);
  
  sock_fd = _try_connect(&server_addr);
  if (sock_fd > 0){
    log_write(LOG_INFO, "from _get_server_connection: connection stablish with server");
    return sock_fd;
  }

  log_write(LOG_ERROR, 
      "from _get_client_connection: the connection process faild");

  return -1;
}


int cc_handle_client_connection(int fd){
  ssize_t data_send;

  int client_fd = fd;
  int server_fd = _get_server_connection(SERVER_PORT, NULL);
  
  for (;;) {
    // client
    log_write(LOG_DEBUG, "client part start 1");
    CmonHttpMessage *req_msg = http_get_message(client_fd);
    if (req_msg->headders_size == 0){
      log_write(LOG_DEBUG, "the client droped the connection");
      return 0;
    }
    log_write(LOG_DEBUG, "client part end 1");

    log_write(LOG_DEBUG, "client part start 2");
    data_send = http_send_http_message(client_fd, server_fd, req_msg);
    if (data_send == 0){
      return 0;
    }
    log_write(LOG_DEBUG, "client end 2");

    // server
    log_write(LOG_DEBUG, "server part start 1");
    CmonHttpMessage *res_msg = http_get_message(server_fd);
    if (res_msg->headders_size == 0){
      log_write(LOG_DEBUG, "the server droped the connection");
      return 0;
    }
    log_write(LOG_DEBUG, "server part end 1");
    
    log_write(LOG_DEBUG, "server part start 2");
    data_send = http_send_http_message(server_fd, client_fd, res_msg);
    if (data_send == 0){
      return 0;
    }
    log_write(LOG_DEBUG, "server part end 2");
  }
}
