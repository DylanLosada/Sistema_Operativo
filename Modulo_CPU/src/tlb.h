#ifndef SRC_TLB_H_
#define SRC_TLB_H_

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "pcb.h"

typedef struct {
	int pagina;
	int marco;
} t_tlb_entry;

int consultar_tlb(t_cpu* cpu, int pagina);
void agregar_entry_tlb(t_cpu* cpu, int pagina, int marco);
void limpiar_tlb(t_cpu* cpu);
void eliminar_marco_tlb(t_cpu* cpu, int marco);

#endif /* SRC_TLB_H_ */
