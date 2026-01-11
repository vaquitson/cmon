#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <logger.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>

#include "sse.h"

int open_serv_sock(uint16_t port){
  struct sockaddr_in server_addr; 

  int fd = socket(AF_INET, SOCK_STREAM, 0);    
  if (fd < 0){
    log_write(LOG_ERROR, "from open_socket: could not open the sokcet -> %s", strerror(errno));
    exit(1);
  }

  bzero(&server_addr, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  server_addr.sin_port = htons(port);

  if (bind(fd ,(struct sockaddr *) &server_addr, sizeof(server_addr)) != 0) {
    log_write(LOG_ERROR, "from open_socket: could not bind the sockaddr to the server -> %s", strerror(errno));
    exit(1); 
  }

  return fd;
}


int open_req_sock(uint16_t serv_port, char *ipv4){
  int sock_fd;
  struct sockaddr_in server_addr; 

  sock_fd = socket(AF_INET, SOCK_STREAM, 0);    

  bzero(&server_addr, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(serv_port);
  if (inet_pton(AF_INET, ipv4, &server_addr.sin_addr) != 1){
    printf("error\n");
    exit(1);
  };
  
  if (connect(sock_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0){
    printf("error\n");
  }

  return sock_fd;
}
