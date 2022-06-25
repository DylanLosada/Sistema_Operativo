#include "memoria.h"


int eliminar_archivo_swap(t_memoria* memoria, t_pcb* pcb_proceso){
	char* path_archivo = obtener_path_swap_del_archivo_del_proceso(pcb_proceso, memoria);
	return remove(path_archivo);
}

char* obtener_path_swap_del_archivo_del_proceso(t_pcb* pcb_cliente, t_memoria* memoria){

	char* path_archivo = memoria->memoria_config->path_swap;
		int id_proceso = pcb_cliente->id;

		char nombre[50];
		strcpy(nombre,  ".swap");
		char* id_proceso_char = string_itoa(id_proceso);

		strcat(id_proceso_char, nombre);
		strcat(path_archivo, id_proceso_char);

		return path_archivo;
}

void hacer_swap_del_proceso(t_pcb* pcb_proceso, t_memoria* memoria){

	char* path_archivo = obtener_path_swap_del_archivo_del_proceso(pcb_proceso, memoria);

	FILE* archivo_proceso;

	archivo_proceso = fopen(path_archivo, "w" ); // VER QUE ESTE FLAG ESTA MAL
	log_info(memoria->memoria_log, "SE ABRE EL ARCHIVO SWAP DEL PROCESO %d", pcb_proceso->id);

	t_tabla_entradas_primer_nivel* tabla_primer_nivel = obtener_tabla_primer_nivel_del_proceso(pcb_proceso, memoria);

	t_list* lista_de_tablas_de_segundo_nivel = tabla_primer_nivel->entradas;

	int tamanio_lista = list_size(lista_de_tablas_de_segundo_nivel);

	int tabla_actual;

	for(tabla_actual = 0; tabla_actual < tamanio_lista; tabla_actual++){
		t_tabla_paginas_segundo_nivel* tabla_pagina_segundo_nivel_iteracion = list_get(lista_de_tablas_de_segundo_nivel, tabla_actual);

		hacer_swap_de_tabla_de_paginas_de_segundo_nivel(tabla_pagina_segundo_nivel_iteracion);

		//CREO QUE HABRIA QUE ACTUALIZAR LA LISTA DE TABLAS DE SEGUNDO NIVEL DE MEMORIA
		//VALE LA PENA TENER ESTA LISTA?? SI TOTAL CADA TABLA TIENE SU LISTA DE TABLAS. RARO

	}

	fclose(archivo_proceso);
}


/*
proceso 1 --> 2 paginas en memoria
1-> 600
2-> 400

CONTENIDO;PAGINA;TABLA2
600;1;1 --> FRAME 3
400;2;1 --> FRAME 500
 */


void hacer_swap_de_tabla_de_paginas_de_segundo_nivel(t_tabla_paginas_segundo_nivel* tabla_pagina_segundo_nivel_iteracion){
	int pagina_actual;
	int cantidad_paginas_de_tabla = list_size(tabla_pagina_segundo_nivel_iteracion->paginas_segundo_nivel);

	for(pagina_actual = 1; pagina_actual < cantidad_paginas_de_tabla + 1; pagina_actual++){

	}

}

int obtener_espacio_de_memoria_a_acceder(t_memoria* memoria, int frame){
	return (memoria->espacio_memoria) + (frame * memoria->memoria_config->tamanio_pagina);
}
