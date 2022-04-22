#include "conexion.h"

typedef struct {
    t_log* log;
    int fd;
    char* server_name;
} t_process_conexion;

void process_connection(void* void_args) {

	t_process_conexion* args = (t_process_conexion*) void_args;
    t_log* logger = args->log;
    int console_socket = args->fd;
    char* kernel_name = args->server_name;
    free(args);

    op_code cop;

    while (console_socket != -1) {


        switch (cop) {
            /*case DEBUG:
                log_info(logger, "debug");
                break;
            */

            case REALIZAR_INSTRUCCIONES:{
                char* instrucciones;

                // RECIBO LAS INSTRUCCIONES //
                //t_list* lista_instrucciones = recibir_paquete(console_socket);



                // MOSTRANDO INSTRUCCIONES //

                log_info(logger, "Instrucciones recibidas: ");
                //log_info(logger, lista_instrucciones);
                //show_instructions(instrucciones);


                // ACA VA LO QUE SE HACE CON LAS INSTRUCCIONES //

                free(instrucciones);
                break;
            }

            // Errores
            case -1:
                log_error(logger, "Consola desconectado de %s...", kernel_name);
                return;
            default:;
                //log_error(logger, "Algo anduvo mal en el %s", kernel_name);
                //log_info(logger, "Cop: %d", cop);

            	t_paquete* paquete = malloc(sizeof(t_paquete));
				recibir_operacion(console_socket, paquete, logger);
				char* mensaje = recibir_buffer(console_socket, paquete);
				log_info(logger, "Me llego el mensaje %s", mensaje);
				log_info(logger, "Tamanio de proceso %d", paquete->processSize);
				free(paquete->buffer->stream);
				free(paquete->buffer);
				free(paquete);
        }
    }

    log_warning(logger, "La consola se desconecto del %s ", kernel_name);
    return;
}

int bind_kernel(t_log* logger, char* kernel_name, int kernel_socket) {
    int socket_consola = wait_console(logger, kernel_name, kernel_socket);

    if (socket_consola > 0) {

    	// CREACION DE HILO //
        pthread_t hilo;
        t_process_conexion* args = malloc(sizeof(t_process_conexion));
        args->log = logger;
        args->fd = socket_consola;
        args->server_name = kernel_name;

        // SE PROCESA LA CONEXION //
        pthread_create(&hilo, NULL, process_connection, args);
        pthread_detach(hilo);
        return 1;
    }
    return 0;
}
