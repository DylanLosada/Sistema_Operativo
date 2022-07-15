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

	//CARGAR LA PAG A MEMORIA pongo el sleep aca para solucionar el tema de que solo haya un sleep de swap por swap de proceso entero
	log_info(memoria->memoria_log, "RESWAP PAGINA (page fault): buscando PAGINA %d EN ARCHIVO (proceso %d). Simulando retardo.", get_numero_pagina_real(pagina), pcb_id);
	sleep(memoria->memoria_config->retardo_memoria/1000 + memoria->memoria_config->retardo_swap/1000);
	sacar_pagina_de_archivo(pcb_id, memoria, marco, pagina);
}

t_marco* get_marco_puntero(t_tabla_entradas_primer_nivel* tabla_1er_nivel) {
	return list_get(tabla_1er_nivel->marcos_usados, tabla_1er_nivel->puntero_clock);
}

void aumentar_puntero(t_tabla_entradas_primer_nivel* tabla_1er_nivel, int marcos_por_proceso) {
	tabla_1er_nivel->puntero_clock = (tabla_1er_nivel->puntero_clock + 1) % marcos_por_proceso;
}

void clock_algoritmo(t_memoria* memoria, t_tabla_entradas_primer_nivel* tabla_1er_nivel, t_pagina_segundo_nivel* pagina_sin_frame, int* marco_to_swap, int pcb_id){

	if(memoria->memoria_config->algoritmo_reemplazo == CLOCK){
		log_info(memoria->memoria_log, "CLOCK: iniciado");
	}else{
		log_info(memoria->memoria_log, "CLOCK-M: iniciado");
	}

	int marcos_por_proceso = memoria->memoria_config->marcos_proceso;
	t_marco* marco_a_desalojar = get_marco_puntero(tabla_1er_nivel);


	if (memoria->memoria_config->algoritmo_reemplazo == CLOCK) {
		// CLOCK
		while (1) {
			if (marco_a_desalojar->pagina->uso == 1) marco_a_desalojar->pagina->uso = 0;
			else {
				aumentar_puntero(tabla_1er_nivel, marcos_por_proceso);
				break;
			}

			aumentar_puntero(tabla_1er_nivel, marcos_por_proceso);
			marco_a_desalojar = get_marco_puntero(tabla_1er_nivel);
		}

	} else {
		// CLOCK-M

		int encontrado = 0;

		// En estos for loops buscamos 0-0 o 0-1 (uso-modificado) 2 vueltas (y modificamos bit de uso cuando buscamos 0-1).
		for (int vuelta = 0; vuelta < 2; vuelta++) {


			for (int i = 0; i < marcos_por_proceso; i++) {
				if (marco_a_desalojar->pagina->uso == 0 && marco_a_desalojar->pagina->modificado == 0) {
					encontrado = 1;
					aumentar_puntero(tabla_1er_nivel, marcos_por_proceso);
					break;
				}
				aumentar_puntero(tabla_1er_nivel, marcos_por_proceso);
				marco_a_desalojar = get_marco_puntero(tabla_1er_nivel);
			}
			if (encontrado) break;
			for (int i = 0; i < marcos_por_proceso; i++) {
				if (marco_a_desalojar->pagina->uso == 0) {
					encontrado = 1;
					aumentar_puntero(tabla_1er_nivel, marcos_por_proceso);
					break;
				}
				marco_a_desalojar->pagina->uso = 0;
				aumentar_puntero(tabla_1er_nivel, marcos_por_proceso);
				marco_a_desalojar = get_marco_puntero(tabla_1er_nivel);
			}
			if (encontrado) break;

		}

	}


	if(marco_a_desalojar->pagina->modificado == 1){

		//pongo el sleep aca para solucionar el tema de que solo haya un sleep de swap por swap de proceso entero
		log_info(memoria->memoria_log, "SWAP PAGINA: actualizando PAGINA %d EN ARCHIVO (proceso %d). Simulando retardo.", get_numero_pagina_real(marco_a_desalojar->pagina), pcb_id);
		sleep(memoria->memoria_config->retardo_memoria/1000 + memoria->memoria_config->retardo_swap/1000);
		swapear_pagina_en_disco(pcb_id, memoria, marco_a_desalojar, marco_a_desalojar->pagina);
		*marco_to_swap = marco_a_desalojar->numero_marco;
	}

	marco_a_desalojar->pagina->presencia = 0;

	log_info(memoria->memoria_log, "CLOCK: PAGINA %d REEMPLAZADA POR PAGINA %d", get_numero_pagina_real(marco_a_desalojar->pagina), get_numero_pagina_real(pagina_sin_frame));
	pagina_sin_frame->marco_usado = marco_a_desalojar;
	marco_a_desalojar->pagina = pagina_sin_frame;

	pagina_sin_frame->presencia = 1;

	//CARGAR LA PAG A MEMORIA pongo el sleep aca para solucionar el tema de que solo haya un sleep de swap por swap de proceso entero
	log_info(memoria->memoria_log, "RESWAP PAGINA (page fault): buscando PAGINA %d EN ARCHIVO (proceso %d). Simulando retardo.", get_numero_pagina_real(pagina_sin_frame), pcb_id);
	sleep(memoria->memoria_config->retardo_memoria/1000 + memoria->memoria_config->retardo_swap/1000);
	sacar_pagina_de_archivo(pcb_id, memoria, marco_a_desalojar, pagina_sin_frame);
}

int get_numero_pagina_real(t_pagina_segundo_nivel* pagina){
	int numero_pagina = pagina->id_pagina;
	int numero_tabla_segundo_nivel = pagina->tabla_segundo_nivel;

	while(numero_tabla_segundo_nivel>0){
		numero_pagina = numero_pagina + 4;
		numero_tabla_segundo_nivel = numero_tabla_segundo_nivel-1;
	}

	return numero_pagina;
}
