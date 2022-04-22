#include "utils.h"

void enviar_mensaje(char* mensaje, int socket_cliente, int processSize)
{
	serializar_paquete(mensaje, processSize, socket_cliente);
}

void serializar_paquete(char* mensaje, int processSize, int socket_cliente)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));
	int size_mensaje = strlen(mensaje)+ 1;

	// Primero completo la estructura buffer interna del paquete.
	t_buffer* buffer = malloc(sizeof(t_buffer));

	buffer->size = size_mensaje + sizeof(int); // La longitud del string nombre. Le sumamos 1 para enviar tambien el caracter centinela '\0'. Esto se podría obviar, pero entonces deberíamos agregar el centinela en el receptor.

	void* stream = malloc(buffer->size);
	int offset = 0; // Desplazamiento

	memcpy(stream, &size_mensaje, sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, mensaje, size_mensaje);

	buffer->stream = stream;

	// Segundo: completo el paquete.
	paquete->processSize = processSize;
	paquete->buffer = buffer;

	void* a_enviar = malloc(buffer->size + sizeof(int) + sizeof(int));
	offset = 0;

	memcpy(a_enviar + offset, &paquete->processSize, sizeof(int));
	offset += sizeof(int);
	memcpy(a_enviar + offset, &paquete->buffer->size, sizeof(int));
	offset += sizeof(int);
	memcpy(a_enviar + offset, paquete->buffer->stream, paquete->buffer->size);

	send(socket_cliente, paquete, buffer->size + sizeof(int) + sizeof(int), 0);

	free(a_enviar);
	free(mensaje);
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
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


void crear_buffer(t_paquete* paquete)
{
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = 0;
	paquete->buffer->stream = NULL;
}

t_paquete* crear_super_paquete(int processSize)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));

	//descomentar despues de arreglar
	paquete->processSize = processSize;
	//crear_buffer(paquete);
	crear_buffer(paquete);
	return paquete;
}

t_paquete* crear_paquete(int processSize)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->processSize = processSize;
	crear_buffer(paquete);
	return paquete;
}

void agregar_a_paquete(t_paquete* paquete, void* valor, int tamanio)
{
	paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->size + tamanio + sizeof(int));

	memcpy(paquete->buffer->stream + paquete->buffer->size, &tamanio, sizeof(int));
	memcpy(paquete->buffer->stream + paquete->buffer->size + sizeof(int), valor, tamanio);

	paquete->buffer->size += tamanio + sizeof(int);
}

void eliminar_paquete(t_paquete* paquete)
{
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}

void liberar_conexion(int socket_cliente)
{
	close(socket_cliente);
}
