#include "funciones.h"


// INICIA SERVER ESCUCHANDO EN IP:PUERTO
int start_kernel(t_kernel* kernel){
	int socket_servidor;

	    struct addrinfo hints, *servinfo, *p;

	    memset(&hints, 0, sizeof(hints));
	    hints.ai_family = AF_UNSPEC;
	    hints.ai_socktype = SOCK_STREAM;
	    hints.ai_flags = AI_PASSIVE;

	    getaddrinfo(NULL, kernel->kernel_config->PUERTO_ESCUCHA, &hints, &servinfo);

	    // Creamos el socket de escucha del servidor
	    socket_servidor = socket(servinfo->ai_family,
	                                 servinfo->ai_socktype,
	                                 servinfo->ai_protocol);

	    int yes = 1;
	    setsockopt(socket_servidor, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

	    // Asociamos el socket a un puerto
	    bind(socket_servidor, servinfo->ai_addr, servinfo->ai_addrlen);

	    // Escuchamos las conexiones entrantes
	    listen(socket_servidor, SOMAXCONN);

	    freeaddrinfo(servinfo);

	    log_info(kernel->kernel_log, "Kernel listo para recibir instrucciones");

	    return socket_servidor;
}


// ESPERAR CONEXION DE CLIENTE A UN SERVER ABIERTO
int wait_console(t_kernel* kernel){
	// Aceptamos un nuevo cliente

	int socket_consola = accept(kernel->kernel_socket, NULL, NULL);

	log_info(kernel->kernel_log, "Consola conectada a kernel");

	return socket_consola;
}


// CERRAR CONEXION
void release_connection(int* socket_cliente){
    close(*socket_cliente);
    *socket_cliente = -1;
}

t_log* iniciar_logger(char* fileLogname, char* programName)
{
	t_log_level LEVEL_ENUM = LOG_LEVEL_TRACE;
	t_log* nuevo_logger = log_create(fileLogname, programName, 1, LEVEL_ENUM);

	return nuevo_logger;
}

char* recive_buffer(int socket_cliente, t_consola* consolaRecv)
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
    memcpy(mensaje, consolaRecv->stream, instructions_size);
    consolaRecv->stream =+ instructions_size;

    return mensaje;
}

int recive_operation(int socket_cliente, t_log* logger){
	int cod_op;
    if(recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL) > 0)
        log_info(logger, "INFORMACION RECIBIDA CORRECTAMENTE");
    else
    {
        log_info(logger, "INFORMACION RECIBIDA INCORRECTAMENTE");
    }
    return cod_op;
}
















