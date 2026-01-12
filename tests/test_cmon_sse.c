#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>

#include "sse.h"

#define PROXY_PORT 7000
#define SERVER_PORT 3000
#define SERVER_PROXY_BUF_LEN 102400 

void echo_req(int sock_fd){
  ssize_t n;
  char buf[2046];
  while ( (n = read(sock_fd, buf, 2046)) >0) {
    printf("req ->\n%s\n", buf);
  }
}

int test_1(){
  uint16_t port = PROXY_PORT;
  int conn_fd;
  socklen_t client_sockaddr_len;
  int fd = open_serv_sock(port);
  struct sockaddr_in clinent_addr; 
  bzero(&clinent_addr, sizeof(clinent_addr));

  listen(fd, 0);  
  while (1){
    conn_fd = accept(fd, (struct sockaddr *)&clinent_addr, &client_sockaddr_len);
    echo_req(conn_fd);
  }
  return 0;
}

int test_2(){
  return 0;
}

int main(void){
  char server_proxy_buf[SERVER_PROXY_BUF_LEN];
  char client_proxy_buf[1024];
  int req_fd;
  size_t read_size;

  int server_proxy_fd = open_req_sock(SERVER_PORT, "192.168.4.120");
  int client_proxy_fd = open_serv_sock(PROXY_PORT);
  int client_proxy_connection_fd;

  socklen_t client_sockaddr_len;
  struct sockaddr_in clinent_addr; 
  bzero(&clinent_addr, sizeof(clinent_addr));

  listen(client_proxy_fd, 0);
  while (1) { 
    client_proxy_connection_fd = accept(client_proxy_fd, (struct sockaddr *)&clinent_addr, &client_sockaddr_len);
    if (client_proxy_connection_fd < 0){
      printf("error: %s\n", strerror(errno));
      exit(errno);
    }

    printf("client request\n");
    read_size = read(client_proxy_connection_fd, client_proxy_buf, 1024);
    printf("%s", client_proxy_buf);
    write(server_proxy_fd, client_proxy_buf, read_size);

    printf("\n\n");
    printf("hello\n");

    printf("server response\n");
    while ((read_size = read(server_proxy_fd, server_proxy_buf, 1024)) > 0){
      write(client_proxy_connection_fd, server_proxy_buf, read_size);  
      printf("%s", server_proxy_buf);
    }
    printf("\n\n");
  }
}
