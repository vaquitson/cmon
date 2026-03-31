#include <string.h>
#include <stdio.h>

#include "cmon_sockets.h"
#include "c_utils_buffer.h"
#include "cmon_http.h"

int test_1(){
  char *expected_header_value = " text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8";

  const char *req =
    "GET / HTTP/1.1\r\n"
    "Host: localhost:7000\r\n"
    "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:148.0) Gecko/20100101 Firefox/148.0\r\n"
    "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n"
    "Accept-Language: en-US,en;q=0.9\r\n"
    "Accept-Encoding: gzip, deflate, br, zstd\r\n"
    "Sec-GPC: 1\r\n"
    "Connection: keep-alive\r\n"
    "Upgrade-Insecure-Requests: 1\r\n"
    "Sec-Fetch-Dest: document\r\n"
    "Sec-Fetch-Mode: navigate\r\n"
    "Sec-Fetch-Site: none\r\n"
    "Priority: u=0, i\r\n"
    "\r\n";

  int rc;

  char buf[300];

  rc = c_http_get_header(req, 
                         strlen(req),
                         "Accept",
                         strlen("Accept"),
                         buf,
                         300);

  if (rc < 0){
    printf("test_1: the c_http_get_header function retunr an unexpected rc: expect 0 get: %d\n", rc);
    return -1;
  }

  buf[rc] = '\0';
  if (memcmp(buf, expected_header_value, rc) != 0){
    printf("test_1: the c_http_get_header function dosen't have the write data, expect ' text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8', but get '%s'\n", buf);
    return -2;
  }
                         
  return 0;
}

int test_2()
{
  int rc;
  int listening_fd; 

  CmonBuffer *header_buf;
 
  socklen_t client_addr_len;
  struct sockaddr_in client_addr;

  header_buf = c_u_buffer_empty(300);
  // test with enought space
  // header_buf = c_u_buffer_empty(1000);

  listening_fd = c_sockets_get_listening_socket(9000);
  if (listening_fd < 0){
    return -1;
  }
  
  printf("listening on 9000\n");
  rc = accept(listening_fd, (struct sockaddr *)&client_addr, &client_addr_len);
  if (rc == -1){
    return -1;
  }
  
  rc = c_http_recv_header1(rc, header_buf);
  if (rc != C_HTTP_SUCESS){
    printf("%d\n", rc);
    return -1;
  }

  c_u_buffer_print(header_buf);
  printf("len: %ld\n", header_buf->len);
  printf("cap: %ld\n", header_buf->cap);

  return 0;
}


int main(){
  if (test_1() == 0){
    printf("Sucesse\n");
  }
  if (test_2() == 0){
    printf("Sucesse\n");
  }
  return 0;
}
