#include "fetch.h"

void fetch(t_pcb* pcb){
	t_log* logger = log_create("cpu.log", "CPU_FETCH", 1, LOG_LEVEL_DEBUG);

	t_instruct* instruct = pcb->instrucciones->head->data;
	log_info(logger, "PC %d, instruccion encontrada: %d (parametros %d %d)", pcb->pc, instruct->op_code, instruct->param1, instruct->param2);
	pcb->pc++;
}
