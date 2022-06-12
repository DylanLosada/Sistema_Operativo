#include "funciones.h"


// INICIA SERVER ESCUCHANDO EN IP:PUERTO
int start_kernel(t_kernel* kernel){
	return create_server_connection(kernel->kernel_config->PUERTO_ESCUCHA, kernel->kernel_log, "KERNEL LISTO PARA RECIBIR INSTRUCCIONES");
}


// ESPERAR CONEXION DE CLIENTE A UN SERVER ABIERTO
int wait_console(t_kernel* kernel){
	// Aceptamos un nuevo cliente

	int socket_consola = accept(kernel->kernel_socket, NULL, NULL);

	log_info(kernel->kernel_log, "SE CONECTO UNA CONSOLA AL KERNEL, ID: %d", socket_consola);

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
















