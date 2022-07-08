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

	archivo_proceso = fopen(path_archivo, "r+");
	log_info(memoria->memoria_log, "SE ABRE EL ARCHIVO SWAP DEL PROCESO %d", pcb_proceso->id);

	t_tabla_entradas_primer_nivel* tabla_primer_nivel = obtener_tabla_primer_nivel_del_proceso(pcb_proceso, memoria);

	t_list* lista_de_id_de_tablas_de_segundo_nivel = tabla_primer_nivel->entradas;

	int tamanio_lista = list_size(lista_de_id_de_tablas_de_segundo_nivel);

	int tabla_actual;

	for(tabla_actual = 0; tabla_actual < tamanio_lista; tabla_actual++){
		int id_tabla_pagina_segundo_nivel_iteracion = list_get(lista_de_id_de_tablas_de_segundo_nivel, tabla_actual);
		t_tabla_paginas_segundo_nivel* tabla_pagina_segundo_nivel_iteracion = obtener_tabla_segundo_nivel_del_proceso(id_tabla_pagina_segundo_nivel_iteracion,  memoria);

		hacer_swap_de_tabla_de_paginas_de_segundo_nivel(tabla_primer_nivel, tabla_pagina_segundo_nivel_iteracion, archivo_proceso, memoria);

	}

	agregar_frames_libres_del_proceso_a_lista_global(tabla_primer_nivel, memoria);

	fclose(archivo_proceso);
}

t_tabla_paginas_segundo_nivel* obtener_tabla_segundo_nivel_del_proceso(int id_tabla_pagina_segundo_nivel, t_memoria* memoria){

	// TODO: refactor/ recorrer marcos_usados de la tabla de primer nivel (en vez de recorrer tabla por tabla y hacerlo con paginas en presencia 1).
	t_list* tablas_segundo_nivel_del_sistema = memoria->tablas_segundo_nivel;

	int tamanio_lista_segundo_nivel = list_size(tablas_segundo_nivel_del_sistema);

	int tabla_actual = 0;
	for(tabla_actual = 0; tabla_actual < tamanio_lista_segundo_nivel; tabla_actual++){
		if(list_get(tablas_segundo_nivel_del_sistema, tabla_actual)!= NULL){
			t_tabla_paginas_segundo_nivel* tabla_pagina_segundo_nivel_de_la_iteracion = list_get(tablas_segundo_nivel_del_sistema, tabla_actual);
			int id_tabla_de_la_iteracion = tabla_pagina_segundo_nivel_de_la_iteracion->id_tabla;
			if(id_tabla_pagina_segundo_nivel == id_tabla_de_la_iteracion){
				log_info(memoria->memoria_log, "LA TABLA DE SEGUNDO NIVEL DEL PROCESO ES LA TABLA DE ID %d", id_tabla_de_la_iteracion);
				return tabla_pagina_segundo_nivel_de_la_iteracion;
			}
		}

	}
	log_info(memoria->memoria_log, "NO SE ENCONTRO LA TABLA DE SEGUNDO NIVEL DEL PROCESO");
	return NULL;

}
//contenido;numpag;tabla2
//1000010101010101010101 01010110101010110101010101 10101010101010110

void hacer_swap_de_tabla_de_paginas_de_segundo_nivel(t_tabla_entradas_primer_nivel* tabla_primer_nivel, t_tabla_paginas_segundo_nivel* tabla_pagina_segundo_nivel_iteracion, FILE* archivo_proceso, t_memoria* memoria){
	int pagina_actual;
	int id_tabla_segundo_nivel = tabla_pagina_segundo_nivel_iteracion->id_tabla;
	t_list* lista_de_paginas = tabla_pagina_segundo_nivel_iteracion->paginas_segundo_nivel;
	int cantidad_paginas_de_tabla = list_size(lista_de_paginas);

	for(pagina_actual = 0; pagina_actual < cantidad_paginas_de_tabla; pagina_actual++){

		t_pagina_segundo_nivel* pagina_iteracion = list_get(lista_de_paginas, pagina_actual);
		int id_pagina = pagina_iteracion->id_pagina;

		//VER logica por que estaria bueno que si la pagina no esta en memoria, el marco tendria q ser -1
		if(pagina_iteracion->presencia == 1){

			hacer_swap_de_pagina(tabla_primer_nivel, pagina_iteracion, id_tabla_segundo_nivel, archivo_proceso, memoria);

		}

	}

}

void hacer_swap_de_pagina(t_tabla_entradas_primer_nivel* tabla_primer_nivel, t_pagina_segundo_nivel* pagina_iteracion, int id_tabla_segundo_nivel, FILE* archivo_proceso, t_memoria* memoria){

	int marco_memoria_de_pagina = pagina_iteracion->marco_usado->numero_marco;
	int id_pagina = pagina_iteracion->id_pagina;
	int desplazamiento = marco_memoria_de_pagina * memoria->memoria_config->tamanio_pagina;
	void* contenido_de_marco = malloc(memoria->memoria_config->tamanio_pagina);
	memcpy(contenido_de_marco, memoria->espacio_memoria + desplazamiento, memoria->memoria_config->tamanio_pagina);

	fwrite(contenido_de_marco, memoria->memoria_config->tamanio_pagina, 1, archivo_proceso);//DUDASAS
	fwrite((void*)id_pagina, sizeof(int), 1, archivo_proceso);
	fwrite((void*)id_tabla_segundo_nivel, sizeof(int), 1, archivo_proceso);

	free(contenido_de_marco);
	pasar_marco_ocupado_a_marco_libre_global(tabla_primer_nivel, marco_memoria_de_pagina, memoria);

}

void pasar_marco_ocupado_a_marco_libre_global(t_tabla_entradas_primer_nivel* tabla_primer_nivel, int marco_memoria_de_pagina, t_memoria* memoria){
	int tamanio_lista_usados = list_size(tabla_primer_nivel->marcos_usados);
	int marco_actual = 0;

	for(marco_actual = 0; marco_actual < tamanio_lista_usados; marco_actual++){

		t_marco* marco_iteracion = list_get(tabla_primer_nivel->marcos_usados, marco_actual);

		if(marco_iteracion->numero_marco == marco_memoria_de_pagina){
			list_remove(tabla_primer_nivel->marcos_usados, marco_actual);
			list_add(memoria->marcos_libres, marco_iteracion);
		}

	}

}

void agregar_frames_libres_del_proceso_a_lista_global(t_tabla_entradas_primer_nivel* tabla_primer_nivel, t_memoria* memoria){

	t_list* marcos_libres_proceso = tabla_primer_nivel->marcos_libres;
	int cantidad_de_frames_libres = list_size(marcos_libres_proceso);
	int marco_actual;

	for(marco_actual = 0; marco_actual < cantidad_de_frames_libres; marco_actual++){
		t_marco* marco_libre = list_remove(marcos_libres_proceso, marco_actual);
		list_add(memoria->marcos_libres, marco_libre->numero_marco);
		log_info(memoria->memoria_log, "MARCO NUMERO %d AGREGADO A FRAMES LIBRES DEL SISTEMA", marco_libre->numero_marco);

	}
}

//El reswap del proceso lo unico q hace es reservarle frames al proceso
void hacer_reswap_del_proceso(t_pcb* pcb_cliente, t_memoria* memoria){
	int marcos_por_proceso = memoria->memoria_config->marcos_proceso;
	t_tabla_entradas_primer_nivel tabla_primer_nivel_del_proceso = obtener_tabla_primer_nivel_del_proceso(pcb_cliente);

	int marco_iteracion;
	for(marco_iteracion = 0; marco_iteracion < marcos_por_proceso; marco_iteracion++){
		t_marco* marco_asignado = malloc(sizeof(t_marco));
		marco_asignado->numero_marco = obtener_marco_de_memoria(memoria);
		//marco_asignado->pagina ??????????????
	}

}

//El reswap de paginas tiene q leer el archivo del proceso y toda la logica hablada hoy

int obtener_espacio_de_memoria_a_acceder(t_memoria* memoria, int frame){
	return (memoria->espacio_memoria) + (frame * memoria->memoria_config->tamanio_pagina);
}
