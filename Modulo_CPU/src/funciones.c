#include "funciones.h"

void execute_interrupt(void* void_args){
	t_cpu* cpu = (t_cpu*) void_args;

	cpu->interrupt->socket = wait_kernel(cpu->cpu_log, cpu->interrupt->socket, cpu->interrupt->puerto);
}

void execute_dispatch(void* void_args){
	t_cpu* cpu = (t_cpu*) void_args;

	cpu->dispatch->socket = wait_kernel(cpu->cpu_log, cpu->dispatch->socket, cpu->dispatch->puerto);

	pthread_t check_interrupt;

	t_interrupt_message* exist_interrupt = malloc(sizeof(t_interrupt_message));
	exist_interrupt->is_interrupt = malloc(sizeof(bool));

	exist_interrupt->socket = cpu->interrupt->socket;
	*exist_interrupt->is_interrupt = false;

	pthread_create(&check_interrupt, NULL, (void*)recive_interrupt , (void*)exist_interrupt);

	while(1){
		int code;
		//SE RECIBE EL PCB DEL KERNEL
		t_pcb* pcb = deserializate_pcb(cpu->dispatch->socket, &code);
		/*
			if(code < 0){
				//TODO
			}
		*/
		//INICIA EL CICLO DE FETCH AND DECODE
		fetch_and_decode(pcb, cpu, exist_interrupt);
	}

}


int start_cpu(char* puerto, t_log* logger, char* conexion){
	char* mensaje;
	if(strcmp(conexion, "dispatch") == 0){
		mensaje = "PUERTO DISPATCH LISTO PARA RECIBIR INSTRUCCIONES";
	}else{
		mensaje = "PUERTO INTERRUPT LISTO PARA RECIBIR INTERRUPCIONES";
	}

	return create_server_connection(puerto, logger, mensaje);
}


int wait_kernel(t_log* log, int socket, char* puerto){
	// Aceptamos un nuevo cliente
	int socket_kernel = accept(socket, NULL, NULL);

	if(socket_kernel > 0){
		log_info(log,"KERNEL CONECTADO A PUERTO %s", puerto);
	}

	return socket_kernel;
}


