#include <sys/socket.h>
#include <stdio.h>
#include <string.h>

#include "sse.h"
#include <unistd.h>


#define TEST_REQ \
"GET / HTTP/1.1\r\n" \
"Host: localhost:5832\r\n" \
"User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:146.0) Gecko/20100101 Firefox/146.0\r\n" \
"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n" \
"Accept-Language: en-US,en;q=0.5\r\n" \
"Accept-Encoding: gzip, deflate, br, zstd\r\n" \
"Sec-GPC: 1\r\n" \
"Connection: keep-alive\r\n" \
"Cookie: served_banks=WzMsMiwwXQ==\r\n" \
"Upgrade-Insecure-Requests: 1\r\n" \
"Sec-Fetch-Dest: document\r\n" \
"Sec-Fetch-Mode: navigate\r\n" \
"Sec-Fetch-Site: none\r\n" \
"Priority: u=0, i\r\n" \
"\r\n"


void echo_req(int sock_fd){
  ssize_t n;
  char buf[2046];
  while ( (n = read(sock_fd, buf, 2046)) >0) {
    printf("req ->\n%s\n", buf);
  }
}

int test_1(){
  uint16_t port = 5832;
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

int main(void){
  uint16_t port = 3000;
  int req_fd;

  req_fd = open_req_sock(port, "192.168.4.120");
  write(req_fd, TEST_REQ, strlen(TEST_REQ));
}
