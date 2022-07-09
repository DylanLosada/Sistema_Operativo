#ifndef SRC_MMU_H_
#define SRC_MMU_H_

#include <math.h>
#include "tlb.h"
#include "config_cpu.h"
#include "pcb.h"

int dir_logica_a_fisica(t_cpu* cpu, t_pcb* pcb, int dir_logica, op_memoria_message instructions_code);
int get_tabla_marco_from_memoria(t_cpu* cpu, int tabla_nivel, int entrada_tabla_nivel, int OPERACION);

#endif /* SRC_MMU_H_ */
