#include "utils.h"

int send_instructions(char* instructions, int socket_cliente, int processSize)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));
	void* instructionsToSend = serializate_instructions(instructions, processSize, paquete);
	int ret = send(socket_cliente, instructionsToSend, paquete->consola->streamLength + sizeof(int) + sizeof(int), 0);
	free(instructionsToSend);
	eliminar_paquete(paquete);
	return ret;
}

void* serializate_instructions(char* instructions, int processSize, t_paquete* paquete){
	t_consola* consola = malloc(sizeof(t_consola));
	int size_instructions = strlen(instructions)+ 1;

	// Primero completo la estructura buffer interna del paquete.
	consola->processSize = processSize;
	consola->streamLength = size_instructions + sizeof(int) + sizeof(int); // La longitud del string nombre. Le sumamos 1 para enviar tambien el caracter centinela '\0'. Esto se podría obviar, pero entonces deberíamos agregar el centinela en el receptor.
	consola->stream = malloc(consola->streamLength);
	int offset = 0; // Desplazamiento

	memcpy(consola->stream, &consola->processSize, sizeof(int));
	offset += sizeof(int);
	memcpy(consola->stream + offset, &size_instructions, sizeof(int));
	offset += sizeof(int);
	memcpy(consola->stream + offset, instructions, size_instructions);
	offset += size_instructions;

	// Segundo: completo el paquete.
	paquete->op_code = CONSOLA;
	paquete->consola = consola;

	void* a_enviar = malloc(consola->streamLength + sizeof(int) + sizeof(int));
	offset = 0;

	memcpy(a_enviar, &paquete->op_code, sizeof(int));
	offset += sizeof(int);
	memcpy(a_enviar + offset, &paquete->consola->streamLength, sizeof(int));
	offset += sizeof(int);
	memcpy(a_enviar + offset, paquete->consola->stream, paquete->consola->streamLength);
	offset += paquete->consola->streamLength;

	free(instructions);
	return a_enviar;
}

int crear_conexion(char *ip, char* puerto)
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


void eliminar_paquete(t_paquete* paquete)
{
	free(paquete->consola->stream);
	free(paquete->consola);
	free(paquete);
}

void liberar_conexion(int socket_cliente)
{
	close(socket_cliente);
}
