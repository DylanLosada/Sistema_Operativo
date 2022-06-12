#ifndef SRC_EXECUTE_H_
#define SRC_EXECUTE_H_

#include "config_cpu.h"
#include "pcb.h"


typedef struct{
	int totalInstructionsExecuted;
	int timeIO;
	clock_t clock;
} t_dataToKernel;


//void execute(t_instruct* instruction, t_cpu* cpu, int totalInstructionsExecuted);


#endif /* SRC_EXECUTE_H_ */
