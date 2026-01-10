#include <sys/socket.h>
#include <stdio.h>
#include <string.h>

#include "sse.h"
#include <unistd.h>

void echo_req(int sock_fd){
  ssize_t n;
  char buf[2046];
  while ( (n = read(sock_fd, buf, 2046)) >0) {
    printf("req ->\n%s\n", buf);
  }
}

int main(void){
  uint16_t port = 5832;
  int conn_fd;
  socklen_t client_sockaddr_len;
  int fd = open_socket(port);
  struct sockaddr_in clinent_addr; 
  bzero(&clinent_addr, sizeof(clinent_addr));

  listen(fd, 0); 
  
  while (1){
    conn_fd = accept(fd, (struct sockaddr *)&clinent_addr, &client_sockaddr_len);
    echo_req(conn_fd);
  }
  return 0;
}
