#ifndef SRC_FETCH_AND_DECODE_H_
#define SRC_FETCH_AND_DECODE_H_

#include "pcb.h"
#include "execute.h"
#include <commons/log.h>
#include <commons/collections/list.h>


t_instruct* destokenizarInstruction(char* stringInstruction);
t_list* destokenizarInstructions(t_list* listInstructions);
void send_data_to_kernel(t_cpu* cpu, t_pcb* pcb, int mensaje);
void fetch_and_decode(t_pcb* pcb, t_cpu* cpu, t_interrupt_message* exist_interrupt);


#endif /* SRC_FETCH_H_ */
