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

#include "cmon_sockets.h"
#include "s_client_connection.h"
#include "logger.h"

#define PROXY_PORT 7000
#define SSE_SERVER_PORT 7001
#define MAX_EPOLL_EVENTS 15

int test_1(){
  int epoll_fd;
  struct epoll_event epoll_events[2];
  struct epoll_event event_queue[MAX_EPOLL_EVENTS];

  int fd_arr[10];
  int cur_fd_arr_index = 0;

  pthread_t thread_id;

  socklen_t client_addr_len;
  struct sockaddr_in client_addr;

  int listening_fd;
  int sse_listening_fd; 

  epoll_fd = epoll_create1(0);
  if (epoll_fd < 0){
    return -1;
  }

  listening_fd = c_sockets_get_listening_socket(PROXY_PORT); 
  if (listening_fd < 0){
    printf("The listening socket for the proxy could not be open\n");
    return -1;
  }
  epoll_events[0].events = EPOLLIN;
  epoll_events[0].data.fd = listening_fd;  

  sse_listening_fd = c_sockets_get_listening_socket(SSE_SERVER_PORT);
  if (sse_listening_fd < 0){
    printf("The listening socket for the sse server could not be open\n");
    return -1;
  }

  epoll_events[1].events = EPOLLIN;
  epoll_events[1].data.fd = sse_listening_fd;  

  if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listening_fd, epoll_events) < 0){
    printf("the file descriptor could not be added to the epoll instance -> %s\n", strerror(errno));
    return -1;
  }

  if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sse_listening_fd, epoll_events+1) < 0){
    printf("the file descriptor could not be added to the epoll instance -> %s\n", strerror(errno));
    return -1;
  }

  for (;;){
    fd_arr[cur_fd_arr_index] = accept(listening_fd, (struct sockaddr *)&client_addr, &client_addr_len);
    log_write(LOG_INFO, "from test_1: new client accepted");
    //epoll_wait(epoll_fd, event_queue, MAX_EPOLL_EVENTS, -1);
    pthread_create(&thread_id, NULL, cc_handle_client_connection, fd_arr+cur_fd_arr_index);
    cur_fd_arr_index++;
  }
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
