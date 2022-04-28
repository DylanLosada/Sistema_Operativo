#include "conexion.h"

typedef struct {
    t_log* log;
    int fd;
} t_process_conexion;

void process_connection(void* void_args) {

	t_process_conexion* args = (t_process_conexion*) void_args;
    t_log* logger = args->log;
    int console_socket = args->fd;
    free(args);

    op_code cod_op = recibir_operacion(console_socket, logger);
    log_info(logger, "Socket %d", console_socket);
	while (console_socket != -1) {


		switch (cod_op) {
			case CONSOLA:{
				t_consola* consolaRecv = malloc(sizeof(t_consola));
				char* mensaje = recibir_buffer(console_socket, &consolaRecv);

				log_info(logger, "Me llego el mensaje %s", mensaje);
				log_info(logger, "Tamanio de proceso %d", consolaRecv->processSize);
				free(consolaRecv->stream);
				free(consolaRecv);
				break;
			}
			// Errores
			case -1:
				log_error(logger, "Consola desconectado de ...");
				return;
			default:;
				//log_error(logger, "Algo anduvo mal en el %s", kernel_name);
				//log_info(logger, "Cop: %d", cop);
				break;
		}
	}

    log_warning(logger, "La consola se desconecto del kernel ");
    return;
}

int bind_kernel(int kernel_socket, t_log* kernel_logger) {

    int socket_consola = wait_console(kernel_socket, kernel_logger);

    if (socket_consola > 0) {

    	// CREACION DE HILO //
        pthread_t hilo;
        t_process_conexion* args = malloc(sizeof(t_process_conexion));
        args->log = kernel_logger;
        args->fd = socket_consola;

        // SE PROCESA LA CONEXION //
        pthread_create(&hilo, NULL, process_connection, args);
        pthread_detach(hilo);
        return 1;
    }
    return 0;
}
