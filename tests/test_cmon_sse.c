#include <sys/socket.h>
#include <stdio.h>
#include <string.h>

#include "sse.h"

int main(){
  int conn_fd;
  socklen_t client_sockaddr_len;
  int fd = open_socket(5000);
  struct sockaddr_in clinent_addr; 
  bzero(&clinent_addr, sizeof(clinent_addr));

  listen(fd, 0); 
  
  while (1){
    conn_fd = accept(fd, (struct sockaddr *)&clinent_addr, &client_sockaddr_len);
    printf("hello\n");
  }
  return 0;
}
