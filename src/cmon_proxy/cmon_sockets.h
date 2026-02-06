#pragma once
#include <arpa/inet.h>

int c_sockets_get_listening_socket(uint16_t port);
int c_sockets_get_server_connection(const uint16_t port, const char *addr);
