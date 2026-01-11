#pragma once
#include <netinet/in.h>

int open_serv_sock(uint16_t port);
int open_req_sock(uint16_t serv_port, char *ipv4);

