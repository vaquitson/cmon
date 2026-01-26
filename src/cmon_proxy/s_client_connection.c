#include <bits/posix_opt.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>

#include "logger.h"
#include "s_client_connection.h"

#define READ_BUFF_LEN 16384
#define SERVER_PORT 3000

int listener_fd;
uint16_t listener_port;



/*
 * Searches for the pattern `p` in the text `t`.
 *
 * Returns the zero-based index (as `ssize_t`) of the first occurrence of `p` in `t`,
 * or -1 if no match is found.
 */
ssize_t _find_patern(char *p, char *t, size_t t_len){
  size_t p_len = strlen(p);
  
  for (size_t s=0; s <= t_len - p_len; s++){
    for (size_t i=0; i < p_len; i++){
      if (t[s+i] != p[i]){
        break; 
      } else if (i == p_len - 1){
        return s;
      }
    }
  }
  return -1;
}

/*
 * Attempts to open a listening socket on the specified port.
 *
 * On success, returns the file descriptor of the listening socket.
 * On failure, returns a negative value.
 */
int _get_listening_socket(uint16_t port){
  static struct sockaddr_in listener_addr;
  int sd;
  int rc;
  
  sd = socket(AF_INET, SOCK_STREAM, 0);
  if (sd < 0){
    log_write(LOG_ERROR,
        "from _get_listening_socket: could not open a socket -> %s", 
        strerror(errno));
    return -1;
  }
  
  bzero(&listener_addr, sizeof(listener_addr));
  listener_addr.sin_family = AF_INET;
  listener_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  listener_addr.sin_port = htons(port);

  rc = bind(sd, 
      (struct sockaddr *) &listener_addr, 
      sizeof(listener_addr));

  if (rc != 0 ){ 
    log_write(LOG_ERROR, 
        "from _get_listening_socket: could not bind the sockaddr to the server -> %s",
        strerror(errno));
    return -1;
  }

  if (listen(sd, 0) < 0){
    log_write(LOG_ERROR, 
        "from _get_listening_socket: could not start a listening socket -> %s",
       strerror(errno));

    return -2;
  }
  
  log_write(LOG_INFO,
      "from _get_listening_socket: sokcet listening on port %ld",
      port);

  return sd;
}



/*
 * Attempts to connect to the given address, retrying indefinitely until it succeeds.
 *
 * This call blocks with no timeout until a connection is established.
 * Returns the connected socket file descriptor.
 */
int _try_connect(struct sockaddr_in *addr){
  int sock_fd;
  int rc;
  log_write(LOG_INFO,
      "from _get_client_connection: trying to connect to the server");
  for (;;){
    sock_fd = socket(AF_INET, SOCK_STREAM, 0); 
    if (sock_fd < 0){
      log_write(LOG_ERROR,
          "from _get_client_connection: could not start a socket -> %s",
          strerror(errno));
      return -1;
    }
    
    rc = connect(sock_fd, (struct sockaddr *)addr, sizeof(*addr));
    if (rc == 0){
      log_write(LOG_INFO,
          "from _try_connect: connection stablish");
      return sock_fd;
    }
    close(sock_fd);
  }
}


/*
 * Establishes a TCP connection to the given address and port and returns the
 * connected socket file descriptor.
 *
 * If `address` is NULL, the function connects to localhost.
 * This call blocks (no timeout) until the connection is established.
 *
 * Returns:
 *   - a non-negative file descriptor on success
 *   - -1 on failure
 */
int _get_server_connection(const uint16_t port, const char *addr){
  struct sockaddr_in server_addr; 
  int sock_fd;
  const char *ip_addr;

  if (addr == NULL){
    ip_addr = "127.0.0.1";
    log_write(LOG_INFO, 
        "from _get_server_connection: the ip addres is set to localhost");
  } else {
    ip_addr = addr;
  }

  bzero(&server_addr, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);
  inet_pton(AF_INET, ip_addr, &server_addr.sin_addr);
  
  sock_fd = _try_connect(&server_addr);
  if (sock_fd > 0){
    return sock_fd;
  }

  log_write(LOG_ERROR, 
      "from _get_client_connection: the connection process faild");

  return -1;
}


/*
 * Reads from `fd` until a complete HTTP header block is available (terminated
 * by the "\r\n\r\n" sequence) and stores the bytes into `buff`.
 *
 * Returns the length (in bytes) of the HTTP headers, including the terminating
 * "\r\n\r\n" sequence, or -1 on failure.
 *
 * On return, `*read_len` contains the total number of bytes read from `fd` and
 * written into `buff`. This may be larger than the header length if the read
 * operation consumed bytes past the end of the headers (i.e., the beginning of
 * the message body). Those extra bytes are left in `buff` immediately after the
 * headers so the caller can reuse them and avoid losing data.
 */
ssize_t _get_http_headders(int fd, char *buff, size_t buff_len, size_t *read_len){
  ssize_t read_size;
  ssize_t p_offset;
  size_t tot_size = 0;

  for (;;){
    if (buff_len <= 0){
      log_write(LOG_ERROR,
          "from _get_http_headders: the length of the hedders is to large");
      return -1;
    }
    read_size = read(fd, buff, buff_len);
    log_write(LOG_DEBUG, "read completed");
    if (read_size < 0) {
      log_write(LOG_ERROR, 
          "from _get_full_reuqest: read errror -> %s",
          strerror(errno));
      return -1;
    }

    tot_size += tot_size;  
    
    log_write(LOG_DEBUG, "%s", buff);
    if ((p_offset = _find_patern("\r\n\r\n", buff, read_size)) != -1){
      *read_len = tot_size;
      return p_offset + 4;
    } else if ((p_offset = _find_patern("\n\n", buff, read_size)) != -1) {
      *read_len = tot_size;
      return p_offset + 4;
    }

    buff_len -= read_size;
    buff += read_size;
  }
}


int cc_handle_client_connection(int fd){
  char client_buff[sizeof(char) * READ_BUFF_LEN];
  size_t read_len;
  ssize_t headder_len;

  int client_fd = fd;
  //int server_fd = _get_server_connection(SERVER_PORT, NULL);
  log_write(LOG_DEBUG, "prev _get_http_headders");
  headder_len = _get_http_headders(client_fd, client_buff, READ_BUFF_LEN, &read_len);   
  client_buff[headder_len+1] = '\0';
  printf("%s", client_buff);
  return 0;
}
