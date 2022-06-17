#ifndef SRC_FETCH_AND_DECODE_H_
#define SRC_FETCH_AND_DECODE_H_

#include "pcb.h"
#include "execute.h"
#include "check_interrupt.h"
#include <commons/log.h>
#include <commons/collections/list.h>


t_list* destokenizarInstructions(t_list* listInstructions);
t_instruct* destokenizarInstruction(char* stringInstruction);
void fetch_and_decode(t_pcb* pcb, t_cpu* cpu);


#endif /* SRC_FETCH_H_ */
