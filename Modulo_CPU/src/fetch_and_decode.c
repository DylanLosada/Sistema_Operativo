#include "fetch_and_decode.h"


t_list* destokenizarInstructions(t_list* listInstructions){

	t_list* instructionsDestokenizadas = list_create();

	for(int i = 0; i < list_size(listInstructions); i++){

		char* list_elem = list_get(listInstructions, i);
		t_instruct* instruct = destokenizarInstruction(list_elem);
		list_add(instructionsDestokenizadas, instruct);
	}

	return instructionsDestokenizadas;

}


t_instruct* destokenizarInstruction(char* stringInstruction){

	t_instruct* instruction = malloc(sizeof(t_instruct));

	char** vec = string_split(stringInstruction, " ");

	op_memoria_message code = NO_OP;

	bool hasParams = false;
	bool hasOneParam = false;

	if(strcmp(vec[0], "I/O") == 0){
		hasOneParam = true;
		code = I_O;
	}else if(strcmp(vec[0], "READ") == 0){
		hasOneParam = true;
		code = READ;
	}else if(strcmp(vec[0], "WRITE") == 0){
		hasParams = true;
		code = WRITE;
	}else if(strcmp(vec[0], "COPY") == 0){
		hasParams = true;
		code = COPY;
	}else if(strcmp(vec[0], "EXIT") == 0){
		code = EXIT;
	}

	instruction->instructions_code = code;

	if(hasOneParam){
		instruction->param1 = strtol(vec[1], &vec[1], 10);
	}else if(hasParams){
		instruction->param1 = strtol(vec[1], &vec[1], 10);
		instruction->param2 = strtol(vec[2], &vec[2], 10);
	}else{
		instruction->param1 = 0;
		instruction->param2 = 0;
	}

	return instruction;
}

void send_data_to_kernel(int kernel_socket, t_cpu* cpu, t_pcb* pcb, int mensaje){
	t_cpu_paquete* cpu_paquete = malloc(sizeof(t_cpu_paquete));
	void* a_enviar = serializate_pcb(pcb, cpu_paquete, mensaje);
	int response = send_data_to_server(kernel_socket, a_enviar, cpu_paquete->buffer->size + sizeof(int) + sizeof(int));

	if(response < 0){
		error_show("OCURRIO UN PROBLEMA INTENTANDO CONECTARSE CON EL KERNEL, ERROR: IMPOSIBLE CONECTAR");
		exit(1);
	}

	log_info(cpu->cpu_log, "SE HA ENVIADO EL PCB AL KERNEL, ID: %d", pcb->id);
}

void fetch_and_decode(int kernel_socket, t_pcb* pcb, t_cpu* cpu, t_interrupt_message* exist_interrupt){

	t_list* instruccionesDestokenizadas = destokenizarInstructions(pcb->instrucciones);

	t_instruct* instruct = malloc(sizeof(t_instruct));

	clock_t time_excecuted = clock();
	while(pcb->program_counter < list_size(pcb->instrucciones)){
		//FETCH
		instruct = list_get(instruccionesDestokenizadas,pcb->program_counter);

		// DECODE / FETCH OPERANDS
		pcb->program_counter++;
		if(instruct->instructions_code == EXIT || instruct->instructions_code == I_O){

			pcb->time_excecuted_rafaga += clock() - time_excecuted;

			if(instruct->instructions_code == I_O){
				pcb->time_io = instruct->param1;
			}

			cpu->args_io_exit->code = instruct->instructions_code;
			cpu->args_io_exit->pcb = pcb;

			pthread_mutex_unlock(cpu->args_io_exit->mutex_has_io_exit);

			break;
		}

		// EXECUTE
		execute(instruct, cpu, pcb);

		// CHECK INTERRUPT
		pthread_mutex_lock(exist_interrupt->mutex_has_interrupt);
		if(exist_interrupt->is_interrupt){
			//SE ENVIA EL PCB ACTUALIZADO AL KERNEL
			int op_code = INTERRUPT;
			pcb->time_excecuted_rafaga += clock() - time_excecuted;
			send_data_to_kernel(kernel_socket, cpu, pcb, op_code);
			exist_interrupt->is_interrupt = false;
			break;
		}
		pthread_mutex_unlock(exist_interrupt->mutex_has_interrupt);
	}
	free(instruct);
	list_destroy(instruccionesDestokenizadas);
}

