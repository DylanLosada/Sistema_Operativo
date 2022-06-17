#include "fetch_and_decode.h"


t_list* destokenizarInstructions(t_list* listInstructions){

	t_list* instructionsDestokenizadas = list_create();

	for(int i = 0; i < listInstructions->elements_count; i++){
		list_add(instructionsDestokenizadas, destokenizarInstruction(list_get(listInstructions, i)));
	}

	return instructionsDestokenizadas;

}


t_instruct* destokenizarInstruction(char* stringInstruction){

	t_instruct* instruction = malloc(sizeof(t_instruct));

	char** vec = string_split(stringInstruction, " ");

	op_instructions_code code = 0;

	bool hasParams = false;
	bool hasOneParam = false;

	if(strcmp(vec[0], "NO_OP") == 0){
	}else if(strcmp(vec[0], "I/O") == 0){
		hasOneParam = true;
		code++;
	}else if(strcmp(vec[0], "READ") == 0){
		hasOneParam = true;
		code += 2;
	}else if(strcmp(vec[0], "WRITE") == 0){
		hasParams = true;
		code += 3;
	}else if(strcmp(vec[0], "COPY") == 0){
		hasParams = true;
		code += 4;
	}else if(strcmp(vec[0], "EXIT") == 0){
		code += 5;
	}else{

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

void send_data_to_kernel(t_cpu* cpu, t_pcb* pcb, int total_instructions_executed, int mensaje){
	t_cpu_paquete* cpu_paquete = malloc(sizeof(t_cpu_paquete));
	list_take_and_remove(pcb->instrucciones, total_instructions_executed);
	int response = send_data_to_server(cpu->dispatch->socket, serializate_pcb(pcb, cpu_paquete, mensaje), cpu_paquete->buffer->size + sizeof(int));

	if(response < 0){
		error_show("OCURRIO UN PROBLEMA INTENTANDO CONECTARSE CON EL KERNEL, ERROR: IMPOSIBLE CONECTAR");
		exit(1);
	}

	log_info(cpu->cpu_log, "SE HA ENVIADO EL PCB AL KERNEL, ID: %d", pcb->id);
}

void fetch_and_decode(t_pcb* pcb, t_cpu* cpu){

	pcb->instrucciones = destokenizarInstructions(pcb->instrucciones);

	t_instruct* instruct = malloc(sizeof(t_instruct));

	int total_instructions_executed = 0;

	pthread_t check_interrupt;

	t_interrupt_message* exist_interrupt = malloc(sizeof(t_interrupt_message));

	exist_interrupt->socket = cpu->interrupt->socket;
	exist_interrupt->is_interrupt = false;

	pthread_create(&check_interrupt, NULL, (void*)recive_interrupt , (void*)exist_interrupt);


	//START EXECUTE
	while(pcb->program_counter != pcb->instrucciones->elements_count){
		instruct = list_get(pcb->instrucciones,pcb->program_counter);
		if(instruct->instructions_code == COPY){
			fetch_operands(instruct->param2);
		}
		execute(instruct, cpu, pcb, total_instructions_executed, 2);
		total_instructions_executed++;

		//GENERAR HILO PARA CHEQUEAR INTERRUPT
		if(exist_interrupt->is_interrupt){

			//send_data_to_kernel(cpu, pcb, total_instructions_executed);
			exist_interrupt = false;
		}

		pcb->program_counter++;
	}


}

