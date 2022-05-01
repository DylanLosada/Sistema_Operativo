#ifndef PROTOCOLO_H_
#define PROTOCOLO_H_

#include "config_kernel.h"

typedef enum {
    CONSOLA,
} op_code;


bool recv_instructions(int fd, char* instructions);

bool send_debug(int fd);

#endif
