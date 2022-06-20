#include "./mmu.h"

int dir_logica_a_fisica(t_cpu* cpu, t_pcb* pcb, int dir_logica) {

	// Se piden a memoria en handshake inicial de CPU.
	t_mem_config* mem_config = cpu->t_mem_config;


	int tabla_1er_nivel = *pcb->tabla_paginas;
	int numero_pagina = floor(dir_logica / mem_config->size_pagina);


	int resultado_tlb = consultar_tlb(cpu, numero_pagina);
	if (resultado_tlb != -1) return resultado_tlb;


	int entrada_tabla_1er_nivel = floor(numero_pagina / mem_config->cant_entradas_por_tabla);
	int entrada_tabla_2do_nivel = numero_pagina % mem_config->cant_entradas_por_tabla;
	int desplazamiento = dir_logica - numero_pagina * mem_config->size_pagina;


	int tabla_2do_nivel = 0; // Preguntar a memoria, pasando tabla_1er_nivel y entrada_tabla_1er_nivel como datos.
	int marco = 0; // Preguntar a memoria, pasando tabla_2do_nivel y entrada_tabla_2do_nivel como datos.
	agregar_entry_tlb(cpu, numero_pagina, marco);

	return marco;
}
