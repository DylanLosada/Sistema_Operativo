#include "execute.h"

void sendDataToKernel(int totalInstructionsExecuted, int timeIO, clock_t clock, int socket){

	t_buffer* buffer = malloc(sizeof(t_buffer));

	buffer->size = sizeof(int) + sizeof(int) + sizeof(clock_t);

	void* stream = malloc(buffer->size);

	int offset = 0; // Desplazamiento

	memcpy(buffer->stream + offset, &totalInstructionsExecuted, sizeof(int));
	offset += sizeof(int);
	memcpy(buffer->stream + offset, &timeIO, sizeof(int));
	offset += sizeof(int);
	memcpy(buffer->stream + offset, &clock, sizeof(clock_t));

	buffer->stream = stream;

	send(socket, buffer, buffer->size, 0);

	free(buffer);

}

void execute_instruction_read_write(int* requerido, t_cpu* cpu, t_pcb* pcb, t_instruct* instruction) {
	int dir_fisica = dir_logica_a_fisica(cpu, pcb, instruction->param1);
	instruction->param1 = dir_fisica;
	send_data_to_memoria(cpu->mem_config->socket, instruction);
	recive_from_memoria(requerido);
}

void execute_instruction_copy(int* requerido, t_cpu* cpu, t_pcb* pcb, t_instruct* instruction, int dir_fisica_primer_param, int dir_fisica_segundo_param) {
	instruction->param1 = dir_fisica_primer_param;
	instruction->param2 = dir_fisica_segundo_param;
	send_data_to_memoria(cpu->mem_config->socket, instruction);
	recive_from_memoria(requerido);
}


void execute(t_instruct* instruction, t_cpu* cpu, t_pcb* pcb) {
	int retardo = cpu->cpu_config->RETARDO_NOOP;

	switch(instruction->instructions_code){
		case NO_OP:
			sleep(retardo/1000);
			log_info(cpu->cpu_log, "NO_OP ===> SE HA REALIZADO UN SLEEP DE: %d",retardo);
			break;
		case READ: {
			//Se deberá leer el valor de memoria correspondiente a esa dirección lógica e imprimirlo por pantalla
			int valor_leido;
			int dir_fisica = excecute_read(cpu, pcb, instruction, &valor_leido);
			log_info(cpu->cpu_log, "READ ===> SE LEYO LA SIGUIENTE DIRECCION FISICA: %d, ENCONTRANDO EL VALOR: %d", dir_fisica, 7);
			/*int espacio_leido;
			execute_instruction_read_write(&espacio_leido, cpu, pcb, instruction);
			log_info(cpu->cpu_log, "EL ESPACIO DE MEMORIA LEIDO: %d", espacio_leido);*/
			break;
		}
		case COPY:{
			// Se deberá escribir en memoria el valor ubicado en la dirección lógica pasada como segundo parámetro, en la dirección lógica pasada como primer parámetro.
			// A efectos de esta etapa, el accionar es similar a la instrucción WRITE ya que el valor a escribir ya se debería haber obtenido en la etapa anterior.
			excecute_copy(cpu, pcb, instruction);
			/*int dir_fisica_primer_param = dir_logica_a_fisica(cpu, pcb, instruction->param1);
			int dir_fisica_segundo_param = dir_logica_a_fisica(cpu, pcb, instruction->param2);
			execute_instruction_copy(op_code, cpu, pcb, instruction, dir_fisica_primer_param, dir_fisica_segundo_param);*/
			//log_info(cpu->cpu_log, "EL ESPACIO DE MEMORIA %d FUE ESCRITO POR %d", dir_fisica_primer_param, dir_fisica_segundo_param);
			break;
		}
		case WRITE:{
			// Se deberá escribir en memoria el valor del segundo parámetro en la dirección lógica del primer parámetro.
			int dir_fisica = excecute_write(cpu, pcb, instruction);
			log_info(cpu->cpu_log, "WRITE ===> SE ESCRIBIO EN LA SIGUIENTE DIRECCION FISICA: %d, EL SIGUIENTE VALOR %d", dir_fisica, instruction->param2);
			/*char* ok_code = "OK";
			int op_code;
			execute_instruction_read_write(&op_code, cpu, pcb, instruction);
			log_info(cpu->cpu_log, "EL ESPACIO DE MEMORIA FUE ESCRITO: CODIGO %s", ok_code);*/
			break;
		}
	}
}

int excecute_read(t_cpu* cpu, t_pcb* pcb, t_instruct* instruction, int* valor_leido){
	int op_code;
	int dir_fisica = dir_logica_a_fisica(cpu, pcb, instruction->param1);
	void* stream = malloc(2*sizeof(int));
	memcpy(stream, &instruction->instructions_code, sizeof(int));
	memcpy(stream + sizeof(int), &dir_fisica, sizeof(int));
	send_data_to_kernel(cpu->mem_config->socket, &dir_fisica, sizeof(int), 0);
	recv(cpu->mem_config->socket, &op_code, sizeof(int), MSG_WAITALL);
	recv(cpu->mem_config->socket + sizeof(int), valor_leido, sizeof(int), MSG_WAITALL);
	return dir_fisica;
}

int excecute_write(t_cpu* cpu, t_pcb* pcb, t_instruct* instruction){
	int op_code;
	int dir_fisica = dir_logica_a_fisica(cpu, pcb, instruction->param1);
	void* stream = malloc(3*sizeof(int));
	memcpy(stream, &instruction->instructions_code, sizeof(int));
	memcpy(stream + sizeof(int), &instruction->param2, sizeof(int));
	memcpy(stream + sizeof(int), &dir_fisica, sizeof(int));
	send_data_to_kernel(cpu->mem_config->socket, stream, 3*sizeof(int), 0);
	recv(cpu->mem_config->socket, &op_code, sizeof(int), MSG_WAITALL);
	log_info(cpu->cpu_log, "WRITE ===> SE EJECUTO LA ESCRITURA EN LA DIR. %d, VALOR: %d", dir_fisica, instruction->param2);
	return dir_fisica;
}

void excecute_copy(t_cpu* cpu, t_pcb* pcb, t_instruct* instruction){
	int op_code;
	int dir_fisica_first = dir_logica_a_fisica(cpu, pcb, instruction->param1);
	int dir_fisica_second = dir_logica_a_fisica(cpu, pcb, instruction->param2);
	void* stream = malloc(3*sizeof(int));
	memcpy(stream, &instruction->instructions_code, sizeof(int));
	memcpy(stream + sizeof(int), &dir_fisica_first, sizeof(int));
	memcpy(stream + sizeof(int), &dir_fisica_second, sizeof(int));
	send_data_to_kernel(cpu->mem_config->socket, stream, 3*sizeof(int), 0);
	recv(cpu->mem_config->socket, &op_code, sizeof(int), MSG_WAITALL);
	log_info(cpu->cpu_log, "COPY ===> SE EJECUTO EL COPIADO DE DATOS DE %d A %d", dir_fisica_first, dir_fisica_second);
}

void send_data_to_memoria(int socket, t_instruct* instruction){
	t_cpu_paquete* paquete = malloc(sizeof(t_cpu_paquete));
	void* to_send = serialize_mmu_memoria(paquete, instruction->param1, instruction->param2, instruction->instructions_code);
	send_data_to_server(socket, to_send, paquete->buffer->size + sizeof(int) + sizeof(int));
}

void recive_from_memoria(int* requiero, int socket){
	recv(socket, requiero, sizeof(int), MSG_WAITALL);
}
