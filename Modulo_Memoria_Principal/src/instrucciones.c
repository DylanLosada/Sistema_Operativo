#include "instrucciones.h"

int leer_memoria(t_memoria* memoria, int direccion) {
	int valor;
	//log_info(memoria->memoria_log, "EMULAMOS RETARDO DE MEMORIA DE %d SEGUNDOS POR READ", memoria->memoria_config->retardo_memoria/1000);
	sleep(memoria->memoria_config->retardo_memoria/1000);
	memcpy(&valor, memoria->espacio_memoria + direccion, sizeof(int));
	return valor;
}

void escribir_memoria(t_memoria* memoria, int direccion, int valor) {
	//log_info(memoria->memoria_log, "EMULAMOS RETARDO DE MEMORIA DE %d SEGUNDOS POR WRITE", memoria->memoria_config->retardo_memoria/1000);
	sleep(memoria->memoria_config->retardo_memoria/1000);
	memcpy(memoria->espacio_memoria + direccion, &valor, sizeof(int));
}

int copiar_memoria(t_memoria* memoria, int direccion_desde, int direccion_hacia) {
	int valor = leer_memoria(memoria, direccion_desde);
	escribir_memoria(memoria, direccion_hacia, valor);
	return valor;
}

int get_tabla_segundo_nivel(t_memoria* memoria, int id_tabla_primer_nivel, int entrada) {
	int segunda_tabla_id;
	for (int index = 0; index < list_size(memoria->tablas_primer_nivel); index++){
		t_tabla_entradas_primer_nivel* tabla = list_get(memoria->tablas_primer_nivel, index);
		if (tabla->id_tabla == id_tabla_primer_nivel) {
			segunda_tabla_id = list_get(tabla->entradas, entrada);
			break;
		}
	}
	return segunda_tabla_id;
}

int get_marco(t_memoria* memoria, int id_tabla_segundo_nivel, int entrada, int* marco_to_swap, op_memoria_message INSTRUCCION, int pcb_id) {

	t_tabla_paginas_segundo_nivel* tabla= list_get(memoria->tablas_segundo_nivel, id_tabla_segundo_nivel);
	t_pagina_segundo_nivel* pagina = list_get(tabla->paginas_segundo_nivel, entrada);

	if (pagina->presencia == 0) {
		asignar_frame_a_pagina(memoria, tabla->tabla_1er_nivel, pagina, marco_to_swap, pcb_id);
	}

	if (INSTRUCCION == WRITE || INSTRUCCION == COPY) {
		pagina->modificado = 1;
	}

	return pagina->marco_usado->numero_marco;
}
