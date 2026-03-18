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

#include "cmon_proxy.h"
#include "c_utils_str.h"
#include "logger.h"
#include "cmon_sockets.h"


#define PROXY_PORT 7000
#define SSE_SERVER_PORT 7001
#define MAX_EPOLL_EVENTS 15

int test_1(){
  c_proxy_start();
  return 0;
}


int test_2(){
  char text[17] = "hello i am \r\n\r\n";
  printf("%ld\n", c_u_str_find_pattern("\r\n\r\n", strlen("\r\n\r\n"), text, 15));
  return 0;
}

int test_3(){
  int rc;
  char dst[] = "hola 12345";
  char src[] = "perro";

  c_u_str_copy_n(src, 5, dst+5, 5);
  if (strcmp(dst, "hola perro") != 0){
    return -1;
  }

  char dst2[] = "hola 12345";
  char src2[] = "perro1";

  rc = c_u_str_copy_n(src2, 6, dst2+5, 5);
  if (rc != -3){
    return -1;
  }
  return 0;
}

int test_4(){
  int rc;
  int listening_fd; 
  pthread_t thread_id;
  
  socklen_t client_addr_len;
  struct sockaddr_in client_addr;

  listening_fd = c_sockets_get_listening_socket(PROXY_PORT);
  if (listening_fd < 0){
    log_write(LOG_ERROR, "from c_proxy_start: an unexpected error happen geting the listening fd");
    return -1;
  }
  
  for (;;){
    rc = accept(listening_fd, (struct sockaddr *)&client_addr, &client_addr_len);
    if (rc == -1){
      log_write(LOG_ERROR, "from c_proxy_start: an erro ocurre while accepting a connection");
    } else {
      pthread_create(&thread_id, NULL, c_proxy_handle_client_2, &rc);
    }
  }

}

int main(void){
  test_4();
  return 0;
}
