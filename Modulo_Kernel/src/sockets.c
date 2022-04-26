#include "sockets.h"

// INICIA SERVER ESCUCHANDO EN IP:PUERTO
int start_kernel(t_log* logger, const char* name, char* puerto)
{
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
	    log_trace(logger, "Listo para escuchar a mi cliente");

	    return socket_servidor;
}


// ESPERAR CONEXION DE CLIENTE A UN SERVER ABIERTO
int wait_console(t_log* logger, const char* name, int socket_kernel){
	// Aceptamos un nuevo cliente

	int socket_consola = accept(socket_kernel, NULL, NULL);

	log_info(logger, "Consola conectada (a %s)\n", name);

	return socket_consola;
}


// CERRAR CONEXION
void release_connection(int* socket_cliente) {
    close(*socket_cliente);
    *socket_cliente = -1;
}

/*
// HECHAS POR DYLAN
int recibir_operacion(int socket_consola){
	int cod_op;

	if(recv(socket_consola, &cod_op, sizeof(int), MSG_WAITALL) > 0)
		return 1;
	else{
		close(socket_consola);
		return -1;
	}
}



void* recibir_buffer(int* size, int socket_consola){
	void * buffer;

	recv(socket_consola, size, sizeof(int), MSG_WAITALL);
	buffer = malloc(*size);
	recv(socket_consola, buffer, *size, MSG_WAITALL);

	return buffer;
}


t_list* recibir_paquete(int socket_cliente)
{
	int size;
	int desplazamiento = 0;
	void * buffer;
	t_list* instructions = list_create();
	int tamanio;

	buffer = recibir_buffer(&size, socket_cliente);
	while(desplazamiento < size)
	{
		memcpy(&tamanio, buffer + desplazamiento, sizeof(int));
		desplazamiento+=sizeof(int);
		char* valor = malloc(tamanio);
		memcpy(valor, buffer+desplazamiento, tamanio);
		desplazamiento+=tamanio;
		list_add(instructions, valor);
	}
	free(buffer);
	return instructions;
}
*/

/*
t_instructions separate_instuctions(t_list* lista){

	t_list* nueva_lista = NULL;
	t_instructions vector[];

	while(lista != NULL){




		lista = lista->head->next;
	}


}
*/

t_log* iniciar_logger(char* fileLogname, char* programName)
{
	t_log_level LEVEL_ENUM = LOG_LEVEL_TRACE;
	t_log* nuevo_logger = log_create(fileLogname, programName, 1, LEVEL_ENUM);

	return nuevo_logger;
}

char* recibir_buffer(int socket_cliente, t_consola* consolaRecv)
{
	int instructions_size;

    if(recv(socket_cliente, &consolaRecv->streamLength, sizeof(int), MSG_WAITALL) < 0)
        return -1;
    consolaRecv->stream = malloc(consolaRecv->streamLength);
    recv(socket_cliente, consolaRecv->stream, consolaRecv->streamLength, MSG_WAITALL);
    memcpy(&consolaRecv->processSize, consolaRecv->stream, sizeof(int));
    consolaRecv->stream += sizeof(int);
    memcpy(&instructions_size, consolaRecv->stream, sizeof(int));
	consolaRecv->stream += sizeof(int);
    char* mensaje = malloc(instructions_size);
    memcpy(mensaje, consolaRecv->stream, consolaRecv->streamLength);

    return mensaje;
}

int recibir_operacion(int socket_cliente, t_log* logger)
{
	int cod_op;
    if(recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL) > 0)
        log_info(logger, "INFORMACION RECIBIDA CORRECTAMENTE");
    else
    {
        log_info(logger, "INFORMACION RECIBIDA INCORRECTAMENTE");
    }
    return cod_op;
}
















