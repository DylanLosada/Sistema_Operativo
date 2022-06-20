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
		case I_O:
			send_data_to_kernel(cpu, pcb, IO);
			break;
		case READ: {
			//Se deberá leer el valor de memoria correspondiente a esa dirección lógica e imprimirlo por pantalla
			int dir_fisica = dir_logica_a_fisica(cpu, pcb, instruction->param1);
			log_info(cpu->cpu_log, "Se ha ejecutado la instrucciones READ");
			break;
		}
		case COPY:
			// Se deberá escribir en memoria el valor ubicado en la dirección lógica pasada como segundo parámetro, en la dirección lógica pasada como primer parámetro.
			// A efectos de esta etapa, el accionar es similar a la instrucción WRITE ya que el valor a escribir ya se debería haber obtenido en la etapa anterior.
			break;
		}
		case WRITE:{
			// Se deberá escribir en memoria el valor del segundo parámetro en la dirección lógica del primer parámetro.
			break;
		}
		case EXIT:
			send_data_to_kernel(cpu, pcb, INTERRUPT);
			break;
	}

	//log_info(cpu->cpu_log, "INSTRUCCION EJECUTADA: %d", instruction->instructions_code);

}
