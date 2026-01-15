#pragma once

#include <netinet/in.h>

typedef struct {
  int connection_fd; 
} S_ClientConnection;

int s_cc_start(uint16_t port);
int _get_listening_socket(uint16_t port);
int _get_server_connection(uint16_t port);
