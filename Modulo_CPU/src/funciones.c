#include "funciones.h"

void execute_cpu(void* void_args){
	t_info* args = (t_info*) void_args;
	int kernel_socket = wait_kernel(args->socket);
	while(1){
		if(kernel_socket > 0){
			log_info(args->log,"KERNEL CONECTADO A PUERTO: %s",args->puerto);
			break;
		}
	}
}


int start_cpu(char* puerto, t_log* logger){
	return create_server_connection(puerto, logger, "CPU LISTA PARA RECIBIR INSTRUCCIONES");
}


int wait_kernel(int socket){
	// Aceptamos un nuevo cliente
	int socket_kernel = accept(socket, NULL, NULL);

	return socket_kernel;
}


