#include "funciones.h"

void execute_interrupt(void* void_args){
	t_cpu* cpu = (t_cpu*) void_args;

	int kernel_socket = wait_kernel(cpu->cpu_log, cpu->interrupt->socket, cpu->interrupt->puerto);

	if(kernel_socket > 0){
		int op_code;
		while(1){
			recv(kernel_socket, &op_code, sizeof(int), MSG_WAITALL);
			// VER QUE PASA SI ES MENOR QUE 0
			// SEMAFOROS
			pthread_mutex_lock(cpu->exist_interrupt->mutex_has_interrupt);
			cpu->exist_interrupt->is_interrupt = true;
			pthread_mutex_unlock(cpu->exist_interrupt->mutex_has_interrupt);
		}
	}

}

void execute_dispatch(void* void_args){
	t_cpu* cpu = (t_cpu*) void_args;

	int kernel_socket = wait_kernel(cpu->cpu_log, cpu->dispatch->socket, cpu->dispatch->puerto);

	while(1){
		int code;
		//SE RECIBE EL PCB DEL KERNEL
		t_pcb* pcb = deserializate_pcb(kernel_socket, &code);
		/*
			if(code < 0){
				//TODO
			}
		*/
		//INICIA EL CICLO DE FETCH AND DECODE
		fetch_and_decode(pcb, cpu, cpu->exist_interrupt);
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


