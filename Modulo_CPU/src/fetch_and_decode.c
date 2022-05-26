#include "fetch_and_decode.h"
/*
void fetch_and_decode(t_pcb* pcb, t_instruct* instruction){
	t_log* logger = log_create("cpu.log", "CPU_FETCH", 1, LOG_LEVEL_DEBUG);

	// FETCH START
	memcpy(instruction, list_get(pcb->instrucciones, pcb->pc), sizeof(t_instruct));

	log_info(logger, "PC %d, instruccion encontrada: %d (parametros %d %d)", pcb->pc, instruction->op_code, instruction->param1, instruction->param2);
	pcb->pc++;


	// DECODE START: determina si FETCH OPERANDS es necesario (solo para COPY en este TP).
	if (instruction->op_code == COPY) {
		// TODO
	}


}*/
