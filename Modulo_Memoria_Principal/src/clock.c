#include "clock.h"

void asignar_frame_a_pagina(t_memoria* memoria, t_tabla_entradas_primer_nivel* tabla_1er_nivel, t_pagina_segundo_nivel* pagina, int* marco_to_swap, int pcb_id) {

	if (list_size(tabla_1er_nivel->marcos_libres) == 0) return clock_algoritmo(memoria, tabla_1er_nivel, pagina, marco_to_swap, pcb_id);

	tabla_1er_nivel->puntero_clock = 0;

	// Dato: por norma, nunca nos va a tocar pedir un marco y que no haya ninguno disponible.
	t_marco* marco = list_remove(tabla_1er_nivel->marcos_libres, 0);

	marco->pagina = pagina;
	list_add(tabla_1er_nivel->marcos_usados, marco);

	pagina->marco_usado = marco;
	pagina->presencia = 1;

	//CARGAR LA PAG A MEMORIA
	sacar_pagina_de_archivo(pcb_id, memoria, marco, pagina);
}


void clock_algoritmo(t_memoria* memoria, t_tabla_entradas_primer_nivel* tabla_1er_nivel, t_pagina_segundo_nivel* pagina_sin_frame, int* marco_to_swap, int pcb_id){

	log_info(memoria->memoria_log, "EL PROCESO OCUPA TODOS LOS MARCOS POR PROCESO DISPONIBLE, INICIANDO ALGORITMO DE REEMPLAZO");

	int marcos_por_proceso = memoria->memoria_config->marcos_proceso;
	t_marco* marco_usado;
	t_pagina_segundo_nivel* pagina_a_desalojar;

	while (1) {
		marco_usado = list_get(tabla_1er_nivel->marcos_usados, tabla_1er_nivel->puntero_clock);
		pagina_a_desalojar = marco_usado->pagina;


		if (memoria->memoria_config->algoritmo_reemplazo == CLOCK) {

		log_info(memoria->memoria_log, "EL ALGORITMO DE REEMPLAZO DEL SISTEMA ES CLOCK");
			// CLOCK NORMAL
			if (pagina_a_desalojar->uso == 1) pagina_a_desalojar->uso = 0;
			else break;


		} else {

			log_info(memoria->memoria_log, "EL ALGORITMO DE REEMPLAZO DEL SISTEMA ES CLOCK-M");
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
		swapear_pagina_en_disco(pcb_id, memoria, marco_usado, pagina_a_desalojar);
		*marco_to_swap = pagina_a_desalojar->marco_usado->numero_marco;
	}


	pagina_sin_frame->marco_usado = marco_usado;
	marco_usado->pagina = pagina_sin_frame;

	//CARGAR LA PAG A MEMORIA
	sacar_pagina_de_archivo(pcb_id, memoria, marco_usado, pagina_sin_frame);
}
