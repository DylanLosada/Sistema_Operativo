#include "conexion.h"
#define EXIT "EXIT"

t_pre_pcb* create_pre_pcb(t_list* list_intructions, int process_size, int console_socket){
	t_pre_pcb* pre_pcb = malloc(sizeof(t_pre_pcb));

	pre_pcb->processSize = process_size;
	pre_pcb->instructions = list_intructions;
	pre_pcb->pcb_id = console_socket;

	return pre_pcb;
}

t_list* destokenizar_instructions(char* message){
	t_list* list_instructions = malloc(sizeof(t_list));
	char** vec = string_split(message, "|");

	for(int index = 0; index < string_array_size(vec); index++){
		if(strcmp(vec[index], EXIT) != 0){
			list_add(list_instructions, vec[index]);
		}
	}

	return list_instructions;
}

void process_connection(t_process_conexion* args) {

    op_code cod_op = recive_operation(args->fd, args->kernel->kernel_log);

	switch (cod_op) {
		case CONSOLA:{
			t_consola* consolaRecv = malloc(sizeof(t_consola));

			t_list* list_instructions = destokenizar_instructions(recive_buffer(args->fd, consolaRecv));

			t_pre_pcb* pre_pcb = create_pre_pcb(list_instructions, consolaRecv->processSize, args->fd);

			pthread_mutex_lock(args->semaforo);

			queue_push(args->cola_pre_pcb, pre_pcb);

			pthread_mutex_unlock(args->semaforo);


			free(consolaRecv->stream);
			free(consolaRecv);
			break;
		}
		// Errores
		case -1:
			log_error(args->kernel->kernel_log, "Consola desconectado de kernel...");
			return;
		default:;
			//log_error(logger, "Algo anduvo mal en el %s", kernel_name);
			//log_info(logger, "Cop: %d", cop);
			break;
	}

    free(args);

    return;
}

void create_pthread(t_process_conexion* args){
	// CREACION DE HILO //
	pthread_t hilo;

	// SE PROCESA LA CONEXION //
	pthread_create(&hilo, NULL, process_connection, args);
	pthread_detach(hilo);
}

int bind_kernel(t_kernel* kernel, t_process_conexion* process_conecction) {

    int console_socket = wait_console(kernel);

    if (console_socket > 0) {
    	process_conecction->fd = console_socket;
		create_pthread(process_conecction);
        return 1;
    }
    return 0;
}
