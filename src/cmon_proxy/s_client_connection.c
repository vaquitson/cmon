#include <bits/posix_opt.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>

#include "cmon_http.h"
#include "s_client_connection.h"
#include "cmon_sockets.h"
#include "logger.h"

#define READ_BUFF_LEN 16384
#define SERVER_PORT 3000

int listener_fd;
uint16_t listener_port;


