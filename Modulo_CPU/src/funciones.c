#include "funciones.h"


void execute_cpu(void* void_args){
	t_info* args = (t_info*) void_args;
	int kernel_socket = wait_kernel(args->socket);
	while(1){
		if(kernel_socket > 0){
			log_info(args->log,"Kernel conectado al puerto %s",args->puerto);
			break;
		}
	}



}


int start_cpu(char* puerto){
	int socket_cpu;

	struct addrinfo hints, *servinfo, *p;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(NULL, puerto, &hints, &servinfo);

	// Creamos el socket de escucha del servidor
	socket_cpu = socket(servinfo->ai_family,
								 servinfo->ai_socktype,
								 servinfo->ai_protocol);

	// Asociamos el socket a un puerto
	bind(socket_cpu, servinfo->ai_addr, servinfo->ai_addrlen);

	// Escuchamos las conexiones entrantes
	listen(socket_cpu, SOMAXCONN);

	freeaddrinfo(servinfo);


	return socket_cpu;
}


int wait_kernel(int socket){
	// Aceptamos un nuevo cliente
	int socket_kernel = accept(socket, NULL, NULL);

	return socket_kernel;
}
/*
int bind_cpu(t_cpu* cpu){

	switch(cpu->dispatch->code){

		case DISPATCH:
			cpu->dispatch->socket = start_cpu(cpu->dispatch->puerto);
			//kernel_socket = wait_kernel(cpu->socket_dispatch);
			break;


		case INTERRUPT:
			cpu->interrupt->socket = start_cpu(cpu->interrupt->puerto);
			//kernel_socket = wait_kernel(cpu->socket_interrupt);
			break;

		default:
			break;
	}

	for(int i=0; i < 2; i++){

	}
	if (cpu->dispatch->socket > 0 || cpu->interrupt->socket > 0) {


		//log_info(cpu->cpu_log, "Kernel conectado a CPU");

	}
	return 1;
}*/



