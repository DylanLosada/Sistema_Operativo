#ifndef SRC_EXECUTE_H_
#define SRC_EXECUTE_H_

#include "config_cpu.h"
#include "pcb.h"
#include "mmu.h"

typedef struct{
	int totalInstructionsExecuted;
	int timeIO;
	clock_t clock;
} t_dataToKernel;

void sendDataToKernel(int totalInstructionsExecuted, int timeIO, clock_t clock, int socket);
void execute(t_instruct* instruction, t_cpu* cpu, t_pcb* pcb);


#endif /* SRC_EXECUTE_H_ */
