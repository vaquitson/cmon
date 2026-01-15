#include <bits/posix_opt.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <arpa/inet.h>

#include "logger.h"
#include "s_client_connection.h"

int listener_fd;
uint16_t listener_port;

int _get_listening_socket(uint16_t port){
  static struct sockaddr_in listener_addr;
  int sd;
  int rc;
  
  sd = socket(AF_INET, SOCK_STREAM, 0);
  if (sd < 0){
    log_write(LOG_ERROR,
        "from s_cc_open_connection: could not open a socket -> %s", 
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
        "from s_cc_open_connection: could not bind the sockaddr to the server -> %s",
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

int _try_connect(struct sockaddr_in *addr){
  int sock_fd;
  int rc;
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
    log_write(LOG_INFO, 
        "from _try_connect: server connection faild, retry -> %s", 
        strerror(errno));

    close(sock_fd);
    sleep(1);
  }
}


int _get_server_connection(uint16_t port){
  struct sockaddr_in server_addr; 
  int sock_fd;
  char localhost[] = "127.0.0.1";

  bzero(&server_addr, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);
  inet_pton(AF_INET, localhost, &server_addr.sin_addr);
  
  sock_fd = _try_connect(&server_addr);
  if (sock_fd > 0){
    return sock_fd;
  } 
  log_write(LOG_ERROR, 
      "from _get_client_connection: the connection process faild");
  exit(1);
}


