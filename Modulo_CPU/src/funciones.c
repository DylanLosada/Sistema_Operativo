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

void execute_blocked_exit(void* void_args){
	t_args_io_exit* args = (t_args_io_exit*) void_args;
	int kernel_socket = wait_kernel(args->cpu_log, args->socket_cpu, "9000");

	while(1){
		pthread_mutex_lock(args->mutex_has_io_exit);
		int op_code = BLOCKED_FINISHED;
		t_cpu_paquete* paquete = malloc(sizeof(t_cpu_paquete));
		void* pcp_to_send = serializate_pcb(args->pcb, paquete, args->code);
		send(kernel_socket, &op_code, sizeof(int), 0);
		send(args->socket_cpu, pcp_to_send, (paquete->buffer->size + sizeof(int) + sizeof(int)), 0);
	}
}

void execute_dispatch(void* void_args){
	t_cpu* cpu = (t_cpu*) void_args;

	int kernel_socket = wait_kernel(cpu->cpu_log, cpu->dispatch->socket, cpu->dispatch->puerto);

	cpu->args_io_exit->socket_cpu = kernel_socket;
	log_info(cpu->cpu_log, "CONEXION KERNEL: conectado a puerto DISPATCH");
	while(1){
		int code;
		//SE RECIBE EL PCB DEL KERNEL
		log_info(cpu->cpu_log, "CONEXION KERNEL: a la espera de un PCB...");
		t_pcb* pcb = malloc(sizeof(t_pcb));
		pcb = deserializate_pcb(kernel_socket, &code);
		log_info(cpu->cpu_log, "CONEXION KERNEL: PCB %d recibido!", pcb->id);

		if (pcb->is_suspended) {
			pcb->is_suspended = 0;
			log_info(cpu->cpu_log, "TLB: LIMPIADA por SUSPENSION de proceso %d", pcb->id);
			limpiar_tlb(cpu);
		}else if (pcb->id != cpu->last_executed_pcb && cpu->last_executed_pcb != -1) {
			log_info(cpu->cpu_log, "TLB: LIMPIADA por CAMBIO de proceso %d a %d", cpu->last_executed_pcb, pcb->id);
			limpiar_tlb(cpu);
		}

		cpu->last_executed_pcb = pcb->id;

		//INICIA EL CICLO DE FETCH AND DECODE
		fetch_and_decode(kernel_socket, pcb, cpu, cpu->exist_interrupt);
	}

}


int start_cpu(char* puerto, t_log* logger, char* conexion){
	char* mensaje;
	if(strcmp(conexion, "dispatch") == 0){
		mensaje = "CONEXION KERNEL: puerto DISPATCH a la espera de instrucciones";
	}else if (strcmp(conexion, "interrupt") == 0){
		mensaje = "CONEXION KERNEL: puerto INTERRUPT LISTO PARA RECIBIR INTERRUPCIONES";
	}else{
		mensaje = "CONEXION KERNEL: puerto INTERRUPT LISTO PARA RECIBIR INTERRUPCIONES";
	}

	return create_server_connection(puerto, logger, mensaje);
}


int wait_kernel(t_log* log, int socket, char* puerto){
	// Aceptamos un nuevo cliente
	int socket_kernel = accept(socket, NULL, NULL);

	if(socket_kernel > 0){
		log_info(log, "CONEXION KERNEL: KERNEL conectado a PUERTO %s", puerto);
	}

	return socket_kernel;
}


