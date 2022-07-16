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
            log_info(cpu->cpu_log, "NO_OP ===> realizando SLEEP de %dms",retardo);
			sleep(retardo/1000);
			break;
		case READ: {
			//Se deberá leer el valor de memoria correspondiente a esa dirección lógica e imprimirlo por pantalla
			int valor_leido = excecute_read(cpu, pcb, instruction);
			log_info(cpu->cpu_log, "READ ===> valor leido: %d", valor_leido);
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
			log_info(cpu->cpu_log, "WRITE ===> valor %d en direccion fisica %d", instruction->param2, dir_fisica);
			/*char* ok_code = "OK";
			int op_code;
			execute_instruction_read_write(&op_code, cpu, pcb, instruction);
			log_info(cpu->cpu_log, "EL ESPACIO DE MEMORIA FUE ESCRITO: CODIGO %s", ok_code);*/
			break;
		}
	}
}

int excecute_read(t_cpu* cpu, t_pcb* pcb, t_instruct* instruction){
	int valor_leido;
	int offset = 0;
	int dir_fisica = dir_logica_a_fisica(cpu, pcb, instruction->param1, instruction->instructions_code);
	void* stream = malloc(2*sizeof(int));
	memcpy(stream, &instruction->instructions_code, sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, &dir_fisica, sizeof(int));
	send(cpu->mem_config->socket, stream, 2*sizeof(int), 0);
	recv(cpu->mem_config->socket, &valor_leido, sizeof(int), MSG_WAITALL);
	return valor_leido;
}

int excecute_write(t_cpu* cpu, t_pcb* pcb, t_instruct* instruction){
	int op_code;
	int valor = instruction->param2;
	int op_code_write = instruction->instructions_code;
	int dir_fisica = dir_logica_a_fisica(cpu, pcb, instruction->param1, instruction->instructions_code);
	int offset = 0;
	int size = sizeof(int) + sizeof(int) + sizeof(int);
	void* stream = malloc(size);
	memcpy(stream, &op_code_write, sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, &valor, sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, &dir_fisica, sizeof(int));
	send_data_to_server(cpu->mem_config->socket, stream, (sizeof(int) + sizeof(int) + sizeof(int)));
	recv(cpu->mem_config->socket, &op_code, sizeof(int), MSG_WAITALL);
	return dir_fisica;
}

void excecute_copy(t_cpu* cpu, t_pcb* pcb, t_instruct* instruction){
	int op_code;
	int dir_fisica_first = dir_logica_a_fisica(cpu, pcb, instruction->param1, instruction->instructions_code);
	int dir_fisica_second = dir_logica_a_fisica(cpu, pcb, instruction->param2, instruction->instructions_code);
	int offset = 0;
	int size = sizeof(int) + sizeof(int) + sizeof(int);
	void* stream = malloc(size);
	memcpy(stream, &instruction->instructions_code, sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, &dir_fisica_first, sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, &dir_fisica_second, sizeof(int));
	send(cpu->mem_config->socket, stream, 3*sizeof(int), 0);
	recv(cpu->mem_config->socket, &op_code, sizeof(int), MSG_WAITALL);
	log_info(cpu->cpu_log, "COPY ===> de direccion fisica %d A %d", dir_fisica_first, dir_fisica_second);
}
