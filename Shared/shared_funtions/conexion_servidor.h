/*
 * conexion_servidor.h
 *
 *  Created on: Apr 30, 2022
 *      Author: dlosada
 */

#ifndef SHARED_FUNTIONS_CONEXION_SERVIDOR_H_
#define SHARED_FUNTIONS_CONEXION_SERVIDOR_H_

#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include "commons/log.h"
#include<unistd.h>
#include<netdb.h>
#include<commons/collections/list.h>
#include<commons/collections/queue.h>
#include<commons/config.h>
#include <dirent.h>
#include <errno.h>

int create_server_connection(char* puerto, t_log* logger, char* message_connection);
int wait_client(int socket_server, t_log* logger, char* client, char* server);
void close_client_connection(int* socket_cliente);

#endif /* SHARED_FUNTIONS_CONEXION_SERVIDOR_H_ */
