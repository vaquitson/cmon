#pragma once

#include <netinet/in.h>

typedef struct {
  int connection_fd; 
} S_ClientConnection;

int _get_listening_socket(uint16_t port);
int _get_server_connection(const uint16_t port, const char *addr);
int cc_handle_client_connection(int fd);
ssize_t _find_patern(char *p, char *t, size_t t_len);
