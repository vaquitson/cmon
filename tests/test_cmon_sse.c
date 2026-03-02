#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <unistd.h>

#include <pthread.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <sys/epoll.h>
#include <fcntl.h>

#include "s_client_connection.h"
#include "cmon_proxy.h"
#include "c_utils_str.h"


#define PROXY_PORT 7000
#define SSE_SERVER_PORT 7001
#define MAX_EPOLL_EVENTS 15

int test_1(){
  c_proxy_start();
  return 0;
}


int test_2(){
  char text[17] = "hello i am \r\n\r\n";
  printf("%ld\n", _find_patern("\r\n\r\n", text, 15));
  return 0;
}

int test_3(){
  int rc;
  char dst[] = "hola 12345";
  char src[] = "perro";

  u_str_copy_n_chars(src, 5, dst+5, 5);
  if (strcmp(dst, "hola perro") != 0){
    return -1;
  }

  char dst2[] = "hola 12345";
  char src2[] = "perro1";

  rc = u_str_copy_n_chars(src2, 6, dst2+5, 5);
  if (rc != -3){
    return -1;
  }
  return 0;
}

int main(void){
  test_1();
  return 0;
}
