/*
 * conexion_servidor.c
 *
 *  Created on: Apr 30, 2022
 *      Author: dlosada
 */

#include "./conexion_servidor.h"

int create_server_connection(char* puerto, t_log* kernel_logger){
	int socket_servidor;

	struct addrinfo hints, *servinfo, *p;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(NULL, puerto, &hints, &servinfo);

	// Creamos el socket de escucha del servidor
	socket_servidor = socket(servinfo->ai_family,
								 servinfo->ai_socktype,
								 servinfo->ai_protocol);

	// Asociamos el socket a un puerto
	bind(socket_servidor, servinfo->ai_addr, servinfo->ai_addrlen);

	// Escuchamos las conexiones entrantes
	listen(socket_servidor, SOMAXCONN);

	freeaddrinfo(servinfo);

	log_info(kernel_logger, "Kernel listo para recibir instrucciones");

	return socket_servidor;
}

int wait_client(int socket_server, t_log* logger, char* client, char* server){
	// Aceptamos un nuevo cliente

	int socket_client = accept(socket_server, NULL, NULL);

	log_info(logger, "%s conectada a %s", client, server);

	return socket_client;
}

void close_client_connection(int* socket_cliente){
    close(*socket_cliente);
    *socket_cliente = -1;
}
