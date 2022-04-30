#include "sockets.h"


int create_config(t_config_kernel* config, t_log* kernel_logger){
	t_config* cfg = config_create("kernel.config");

	if(cfg == NULL){
		log_error(kernel_logger, "No se ha encontrado kernel.config");
		return 0;
	}


	config->IP_MEMORIA = config_get_string_value(cfg, "IP_MEMORIA");
	config->PUERTO_MEMORIA = config_get_string_value(cfg, "PUERTO_MEMORIA");
	config->IP_CPU = config_get_string_value(cfg, "IP_CPU");
	config->PUERTO_CPU_DISPATCH = config_get_string_value(cfg, "PUERTO_CPU_DISPATCH");
	config->PUERTO_CPU_INTERRUPT = config_get_string_value(cfg, "PUERTO_CPU_INTERRUPT");
	config->PUERTO_ESCUCHA = config_get_string_value(cfg, "PUERTO_ESCUCHA");
	config->ALGORITMO_PLANIFICACION = config_get_string_value(cfg, "ALGORITMO_PLANIFICACION");
	config->ESTIMACION_INICIAL = config_get_string_value(cfg, "ESTIMACION_INICIAL");
	config->ALFA = config_get_string_value(cfg, "ALFA");
	config->GRADO_MULTIPROGRAMACION = config_get_string_value(cfg, "GRADO_MULTIPROGRAMACION");
	config->TIEMPO_MAXIMO_BLOQUEADO = config_get_string_value(cfg, "TIEMPO_MAXIMO_BLOQUEADO");

	//determinar planificacion

	log_info(kernel_logger, "Archivo de configuracion cargado con exito");

	return 1;

}


// INICIA SERVER ESCUCHANDO EN IP:PUERTO
int start_kernel(char* puerto, t_log* kernel_logger){
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


// ESPERAR CONEXION DE CLIENTE A UN SERVER ABIERTO
int wait_console(int socket_kernel, t_log* kernel_logger){
	// Aceptamos un nuevo cliente

	int socket_consola = accept(socket_kernel, NULL, NULL);

	log_info(kernel_logger, "Consola conectada a kernel");

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

int recibir_operacion(int socket_cliente, t_log* logger){
	int cod_op;
    if(recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL) > 0)
        log_info(logger, "INFORMACION RECIBIDA CORRECTAMENTE");
    else
    {
        log_info(logger, "INFORMACION RECIBIDA INCORRECTAMENTE");
    }
    return cod_op;
}
















