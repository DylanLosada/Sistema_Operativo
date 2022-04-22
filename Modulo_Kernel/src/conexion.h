#ifndef CONEXION_H_
#define CONEXION_H_

#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include <inttypes.h>
#include <commons/log.h>

#include "sockets.h"
#include "protocolo.h"


int server_escuchar(t_log* logger, char* server_name, int server_socket);


#endif
