/*
 * conexion_cliente.h
 *
 *  Created on: Apr 30, 2022
 *  Author: dlosada
 */

#ifndef SHARED_FUNTIONS_CONEXION_CLIENTE_H_
#define SHARED_FUNTIONS_CONEXION_CLIENTE_H_

#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>

int create_client_connection(char *ip, char* puerto);
int send_data_to_server(int socket_cliente, void* dataToSend, int sizeDataToSend);
void free_connection(int socket_cliente);

#endif /* SHARED_FUNTIONS_CONEXION_CLIENTE_H_ */
