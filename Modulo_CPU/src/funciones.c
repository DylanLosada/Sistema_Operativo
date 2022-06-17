#include "funciones.h"

void execute_interrupt(void* void_args){
	t_cpu* cpu = (t_cpu*) void_args;

	cpu->interrupt->socket = wait_kernel(cpu->interrupt->socket);
}

void execute_dispatch(void* void_args){
	t_cpu* cpu = (t_cpu*) void_args;

	pthread_t check_interrupt;

	t_interrupt_message* exist_interrupt = malloc(sizeof(t_interrupt_message));
	exist_interrupt->is_interrupt = malloc(sizeof(bool));

	exist_interrupt->socket = cpu->interrupt->socket;
	*exist_interrupt->is_interrupt = false;

	pthread_create(&check_interrupt, NULL, (void*)recive_interrupt , (void*)exist_interrupt);


	cpu->dispatch->socket = wait_kernel(cpu->dispatch->socket);
	log_info(cpu->cpu_log,"KERNEL CONECTADO A PUERTO %s",cpu->dispatch->puerto);

	while(1){
		int code;
		t_pcb* pcb = deserializate_pcb(cpu->dispatch->socket, &code);
		/*
			if(code < 0){
				//TODO
			}
		<<<<<<< Updated upstream
		*/
		fetch_and_decode(pcb, cpu, exist_interrupt);
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


