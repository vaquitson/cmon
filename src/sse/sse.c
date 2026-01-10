#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <logger.h>
#include <errno.h>
#include <string.h>

#include "sse.h"

int open_socket(uint16_t port){
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


void make_request(){

}
