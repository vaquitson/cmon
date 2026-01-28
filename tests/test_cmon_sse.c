#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>

#include "s_client_connection.h"
#include "logger.h"

#define PROXY_PORT 7000
#define SERVER_PROXY_BUF_LEN 102400 

#define HTTP_RES_MIN_HTML \
"HTTP/1.1 200 OK\r\n" \
"Content-Type: text/html; charset=utf-8\r\n" \
"Content-Length: 11\r\n" \
"Connection: close\r\n" \
"\r\n" \
"<h1>hi</h1>"


void echo_req(int sock_fd){
  ssize_t n;
  char buf[3072];

  while ( (n = read(sock_fd, buf, 2046)) >0) {
    printf("req ->\n%s\n", buf);
  }
}

int test_1(){
  uint16_t port = PROXY_PORT;
  uint16_t server_port = 3000;

  socklen_t client_addr_len;
  struct sockaddr_in client_addr;
  int client_conn_fd; 
  int listening_fd, server_conn_fd;

  ssize_t recv_len;
  ssize_t send_len;

  size_t client_buf_len = 3072;
  char client_buff[client_buf_len];

  size_t server_buf_len = 3072;
  char server_buff[client_buf_len];

  listening_fd = _get_listening_socket(port);
  //server_conn_fd = _get_server_connection(server_port, NULL);
    
  ssize_t rc;

  client_conn_fd = accept(listening_fd, (struct sockaddr *)&client_addr, &client_addr_len);
  cc_handle_client_connection(client_conn_fd);

  return 0;
}


int test_2(){
  char text[17] = "hello i am \r\n\r\n";
  printf("%ld\n", _find_patern("\r\n\r\n", text, 15));
  return 0;
}

int main(void){
  test_1();

  return 0;
}
