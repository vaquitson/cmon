#pragma once
#include <arpa/inet.h>
#include <sys/socket.h>

int c_sockets_get_listening_socket(uint16_t port);
int c_sockets_get_server_connection(const uint16_t port, const char *addr);

#define c_sockets_peek(fd, buf, buf_size) recv((fd), (buf), (buf_size), MSG_PEEK)
