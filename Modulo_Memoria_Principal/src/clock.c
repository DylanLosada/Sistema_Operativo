#include "clock.h"

void asignar_frame_a_pagina(t_memoria* memoria, t_tabla_entradas_primer_nivel* tabla_1er_nivel, t_pagina_segundo_nivel* pagina) {

	if (list_size(tabla_1er_nivel->marcos_libres) == 0) return clock(memoria, tabla_1er_nivel, pagina);


	// TODO: si el proceso vuelve a memoria el clock empieza en 0?
	tabla_1er_nivel->puntero_clock = 0;

	// Dato: por norma, nunca nos va a tocar pedir un marco y que no haya ninguno disponible.
	int marco = list_remove(tabla_1er_nivel->marcos_libres, 0); // TODO: free?

	t_marco_usado* marco_usado = malloc(sizeof(t_marco_usado));
	marco_usado->numero_marco = marco;
	marco_usado->pagina = pagina;
	list_add(tabla_1er_nivel->marcos_usados, marco_usado);

	pagina->marco_usado = marco_usado;
}


void clock(t_memoria* memoria, t_tabla_entradas_primer_nivel* tabla_1er_nivel, t_pagina_segundo_nivel* pagina_sin_frame){

	int marcos_por_proceso = memoria->memoria_config->marcos_proceso;
	t_marco_usado* marco_usado;
	t_pagina_segundo_nivel* pagina_a_desalojar;

	while (1) {
		marco_usado = list_get(tabla_1er_nivel->marcos_usados, tabla_1er_nivel->puntero_clock);
		pagina_a_desalojar = marco_usado->pagina;


		if (memoria->memoria_config->algoritmo_reemplazo == CLOCK) {

			// CLOCK NORMAL
			if (pagina_a_desalojar->uso == 1) pagina_a_desalojar->uso = 0;
			else break;


		} else {

			// CLOCK MODIFICADO
			int encontrado = 0;


			// En estos for loops buscamos 0-0 o 0-1 (uso-modificado) 2 vueltas (y modificamos bit de uso cuando buscamos 0-1).
			for (int vuelta = 0; vuelta < 2; vuelta++) {


				for (int i = 0; i < marcos_por_proceso; i++) {
					if (pagina_a_desalojar->uso == 0 && pagina_a_desalojar->modificado == 0) {
						encontrado = 1;
						break;
					}
				}
				if (encontrado) break;
				for (int i = 0; i < marcos_por_proceso; i++) {
					if (pagina_a_desalojar->uso == 0) {
						encontrado = 1;
						break;
					}
					pagina_a_desalojar->uso = 0;
				}
				if (encontrado) break;


			}




		}
		// Se le suma 1 al clock y se pone en 0 si alcanza el maximo.
		tabla_1er_nivel->puntero_clock = (tabla_1er_nivel->puntero_clock + 1) % marcos_por_proceso;
	}


	if (pagina_a_desalojar->modificado == 1) {
		// TODO: --- DESALOJAR pagina_a_desalojar ---
	}


	pagina_sin_frame->marco_usado = marco_usado;
	marco_usado->pagina = pagina_sin_frame;
}
