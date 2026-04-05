#pragma once

#include <stdlib.h>

#include "c_utils_buffer.h"

typedef struct {
  int state;

  int sender;
  int receiver; 

  size_t data_send_size;
  size_t content_length;

  CmonBuffer *header; 
} CmonProxyCommManager;


typedef struct {
  const char *payload;
  size_t payload_size;

  const char *inject_key; 
  size_t inject_key_size;
} CmonProxyInjectionManager;


#define c_proxy_comm_get_sender(com) (com)->sender
#define c_proxy_comm_get_receiver(com) (com)->receiver

#define c_proxy_comm_get_header(com) (com)->header
#define c_proxy_comm_get_header_len(com) c_u_buffer_get_len(c_proxy_comm_get_header((com)))

#define c_proxy_comm_set_header(com, buf) (com)->header = buf

int c_proxy_start(void);
void *c_proxy_communication_manager(void *fd);
