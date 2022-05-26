#include "funciones.h"


void execute_cpu(void* void_args){
	t_info* args = (t_info*) void_args;
	args->socket = start_cpu(args->puerto);
	log_info(args->logger, "CPU escuchando en puerto %s", args->puerto);
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


int wait_kernel(t_cpu* cpu){
	// Aceptamos un nuevo cliente

	int socket_consola = accept(cpu->socket, NULL, NULL);



	return socket_consola;
}

int bind_cpu(t_cpu* cpu, t_info* args){
	int kernel_socket = wait_kernel(cpu);

	if (kernel_socket > 0) {
		log_info(cpu->cpu_log, "Kernel conectado a CPU");
		while(1){

		}
	}
	return 1;
}


