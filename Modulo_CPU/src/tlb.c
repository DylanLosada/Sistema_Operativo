#include "tlb.h"

int consultar_tlb(t_cpu* cpu, int pagina) {
	// Devuelve el marco correspondiente a la pagina, si es que se encuentra.

	t_list* tlb = cpu->tlb;
	t_tlb_entry* entry = NULL;

	for (int i = 0; i < list_size(tlb); i++){
		entry = list_get(tlb, i);
		if (entry->pagina == pagina) {

			// Si el algoritmo de remplazo es LRU remover elemento y agregarlo al final para marcarlo como el ultimo utilizado.
			if (strcmp(cpu->cpu_config->REEMPLAZO_TLB, "LRU") == 0) {
				list_remove(tlb, i);
				list_add(tlb, entry);
			}

			return entry->marco;
		}
	}

	return -1;
}

void agregar_entry_tlb(t_cpu* cpu, int pagina, int marco) {
	t_tlb_entry* entry = malloc(sizeof(t_tlb_entry));
	entry->pagina = pagina;
	entry->marco = marco;

	t_list* tlb = cpu->tlb;

	// Checkeamos si agregar un elemento haria que nos pasemos del maximo de entradas permitidas, y en ese caso eliminamos el primero.
	if (list_size(tlb) >= cpu->cpu_config->ENTRADAS_TLB) {
		t_tlb_entry* first_entry = list_remove(tlb, 0);
		free(first_entry);
	}

	list_add(tlb, entry);
}

void limpiar_tlb(t_cpu* cpu) {
	t_list* tlb = cpu->tlb;
	int size = list_size(tlb);

	for (int i = 0; i < size; i++){
		t_tlb_entry* first_entry = list_remove(tlb, 0);
		free(first_entry);
	}
}

void eliminar_marco_tlb(t_cpu* cpu, int marco) {
    t_list* tlb = cpu->tlb;

    for (int i = 0; i < list_size(tlb); i++){
        t_tlb_entry* entry = list_get(tlb, i);
        if (entry->marco == marco) {
            list_remove(tlb, i);
            free(entry);
            break;
        }
    }
}
