#include <string.h>
#include <stdio.h>

#include "c_utils_str.h"
#include "cmon_http.h"

int test_1(){
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
                         
  c_u_str_print_n(buf, rc);
  return 0;
}


int main(){
  test_1();
  return 0;
}
