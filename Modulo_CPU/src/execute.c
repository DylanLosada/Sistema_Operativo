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

void execute(t_instruct* instruction, t_cpu* cpu, t_pcb* pcb) {
	int retardo = cpu->cpu_config->RETARDO_NOOP;

	switch(instruction->instructions_code){
		case NO_OP:
			sleep(retardo/1000);
			log_info(cpu->cpu_log, "SE HA REALIZADO UN SLEEP DE: %d",retardo);
			break;
		case READ: {
			//Se deberá leer el valor de memoria correspondiente a esa dirección lógica e imprimirlo por pantalla
			int dir_fisica = excecute_read(cpu, pcb, instruction);
			log_info(cpu->cpu_log, "SE LEYO LA SIGUIENTE DIRECCION FISICA: %d", dir_fisica);
			break;
		}
		case COPY:
			// Se deberá escribir en memoria el valor ubicado en la dirección lógica pasada como segundo parámetro, en la dirección lógica pasada como primer parámetro.
			// A efectos de esta etapa, el accionar es similar a la instrucción WRITE ya que el valor a escribir ya se debería haber obtenido en la etapa anterior.
			excecute_copy(cpu, pcb, instruction);
			log_info(cpu->cpu_log, "SE REALIZA LA COPIA");
			break;
		case WRITE:{
			// Se deberá escribir en memoria el valor del segundo parámetro en la dirección lógica del primer parámetro.
			int dir_fisica = excecute_write(cpu, pcb, instruction);
			log_info(cpu->cpu_log, "SE ESCRIBIO EN LA SIGUIENTE DIRECCION FISICA: %d, EL SIGUIENTE VALOR %d", dir_fisica, instruction->param2);
			break;
		}
	}
}

int excecute_read(t_cpu* cpu, t_pcb* pcb, t_instruct* instruction){
	int op_code;
	int dir_fisica = dir_logica_a_fisica(cpu, pcb, instruction->param1);
	void* stream = malloc(2*sizeof(int));
	memcpy(stream, &instruction->instructions_code, sizeof(int));
	memcpy(stream + sizeof(int), &dir_fisica, sizeof(int));
	send_data_to_kernel(cpu->mem_config->socket, &dir_fisica, sizeof(int), 0);
	recv(cpu->mem_config->socket, &op_code, 2*sizeof(int), MSG_WAITALL);
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
}
