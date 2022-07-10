#include "conexion.h"

t_pre_pcb* create_pre_pcb(t_list* list_intructions, int process_size, int console_socket){
	t_pre_pcb* pre_pcb = malloc(sizeof(t_pre_pcb));

	pre_pcb->processSize = process_size;
	pre_pcb->instructions = list_intructions;
	pre_pcb->pcb_id = console_socket;

	return pre_pcb;
}

t_list* destokenizar_instructions(char* message){
	t_list* list_instructions = list_create();
	char** vec = string_split(message, "|");

	for(int index = 0; index < string_array_size(vec); index++){
		if(vec[index] != NULL && strcmp(vec[index], "") != 0){
			list_add(list_instructions, vec[index]);
		}
	}

	//free_instruction_split(vec);
	free(vec);
	return list_instructions;
}

void process_connection(void* void_args) {
	t_structs_process_conexion* args = (t_structs_process_conexion*) void_args;
    op_code cod_op = recive_operation(args->socket, args->structs->kernel->kernel_log);

	switch (cod_op) {
		case CONSOLA:{
			t_consola* consolaRecv = malloc(sizeof(t_consola));
			char* instructions = recive_buffer(args->socket, consolaRecv);
			t_list* list_instructions = destokenizar_instructions(instructions);

			t_pre_pcb* pre_pcb = create_pre_pcb(list_instructions, consolaRecv->processSize, args->socket);

			pthread_mutex_lock(args->structs->semaforo);
			queue_push(args->structs->cola_pre_pcb, pre_pcb);
			pthread_mutex_unlock(args->structs->semaforo);
			pthread_mutex_unlock(args->structs->hasNewConsole);

			free(consolaRecv);
			free(instructions);
			break;
		}
		// Errores
		case -1:
			log_error(args->structs->kernel->kernel_log, "Consola desconectado de kernel...");
			return;
		default:;
			//log_error(logger, "Algo anduvo mal en el %s", kernel_name);
			//log_info(logger, "Cop: %d", cop);
			break;
	}

    return;
}

void create_pthread(t_structs_process_conexion* structs){
	// CREACION DE HILO //
	pthread_t hilo;

	// SE PROCESA LA CONEXION //
	pthread_create(&hilo, NULL, process_connection, structs);
	pthread_detach(hilo);
}

int bind_kernel(t_kernel* kernel, t_process_conexion* args) {

    int console_socket = wait_console(kernel);
    t_structs_process_conexion* structs = malloc(sizeof(t_structs_process_conexion));
    structs->structs = args;
    structs->socket = console_socket;
    if (console_socket > 0) {
		create_pthread(structs);
        return 1;
    }
    return 0;
}
