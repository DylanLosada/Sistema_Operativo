/*
 * conexion_cliente.c
 *
 *  Created on: Apr 30, 2022
 *  Author: dlosada
 */
#include "./conexion_cliente.h";


int create_client_connection(char *ip, char* puerto)
{
	struct addrinfo hints;
	struct addrinfo *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &hints, &server_info);

	// Ahora vamos a crear el socket.
	int socket_cliente = socket(server_info->ai_family,
            server_info->ai_socktype,
            server_info->ai_protocol);


	// Ahora que tenemos el socket, vamos a conectarlo
	connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen);

	freeaddrinfo(server_info);

	return socket_cliente;
}

int send_data_to_server(int socket_cliente, void* dataToSend, int sizeDataToSend){
	return send(socket_cliente, dataToSend, sizeDataToSend, 0);
}

void free_connection(int socket_cliente)
{
	close(socket_cliente);
}
