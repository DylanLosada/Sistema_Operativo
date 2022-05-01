#include "execute.h"

void execute(t_instruct* instruction, int no_op_time) {
	t_log* logger = log_create("cpu.log", "CPU_EXECUTE", 1, LOG_LEVEL_DEBUG);
	t_log* logger_read = log_create("cpu.log", "CPU_READ", 1, LOG_LEVEL_INFO);

	int mem_value;
	switch(instruction->op_code){
		case NO_OP:
			sleep(no_op_time);
			break;
		case I_O:
			// Devolver PCB actualizado a kernel con tiempo de bloqueo en MS.
			break;
		case READ:
			// TODO: Leer de memoria
			mem_value = 0; // Simulando read de memoria
			log_info(logger_read, "%d", mem_value); // Imprimir por pantalla contenido, pedido en consigna.
			break;
		case COPY:
			//
			break;
		case WRITE:
			// Se deberá escribir en memoria el valor del segundo parámetro en la dirección lógica del primer parámetro.
			break;
		case EXIT:
			// Devolver PCB actualizado.
			break;
	}


	log_info(logger, "Executed %d (parametros %d %d)", instruction->op_code, instruction->param1, instruction->param2);

}
