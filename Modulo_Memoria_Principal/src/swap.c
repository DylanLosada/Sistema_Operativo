#include "memoria.h"

int eliminar_archivo_swap(t_memoria* memoria, t_pcb* pcb_proceso){
	char* path_archivo = obtener_path_swap_del_archivo_del_proceso(pcb_proceso->id, memoria);
	return remove(path_archivo);
}

char* obtener_path_swap_del_archivo_del_proceso(int pcb_id, t_memoria* memoria){

	char* path_archivo = memoria->memoria_config->path_swap;
	int id_proceso = pcb_id;

	char nombre[10];
	strcpy(nombre,  ".swap");
	char* id_proceso_char = string_itoa(id_proceso);

	strcat(id_proceso_char, nombre);
	char* path_absoluto = malloc(strlen(path_archivo) + strlen(id_proceso_char));
	strcpy(path_absoluto, path_archivo);
	strcat(path_absoluto, id_proceso_char);

	return path_absoluto;
}

void hacer_swap_del_proceso(t_pcb* pcb_proceso, t_memoria* memoria){

	//char* path_archivo = obtener_path_swap_del_archivo_del_proceso(pcb_proceso->id, memoria);

	//FILE* archivo_proceso;

	//archivo_proceso = fopen(path_archivo, "rb");
	log_info(memoria->memoria_log, "SWAP (proceso %d): Guardando TODAS las PAGINAS MODIFICADAS. Simulando retardo.", pcb_proceso->id);
	sleep(memoria->memoria_config->retardo_swap/1000);

	t_tabla_entradas_primer_nivel* tabla_primer_nivel = obtener_tabla_primer_nivel_del_proceso(pcb_proceso, memoria);

	for(int index = 0; index < list_size(tabla_primer_nivel->marcos_usados); index++){

		t_marco* marco_iteracion = list_get(tabla_primer_nivel->marcos_usados, index);

		t_pagina_segundo_nivel* pagina_iteracion = marco_iteracion->pagina;

		pagina_iteracion->presencia = 0;
		pagina_iteracion->uso = 0;
		pagina_iteracion->modificado = 0;

		swapear_pagina_en_disco(pcb_proceso->id, memoria, marco_iteracion, pagina_iteracion);

		marco_iteracion->pagina = NULL;
	}
	pasar_marco_ocupado_a_marco_libre_global(tabla_primer_nivel, memoria);
	agregar_frames_libres_del_proceso_a_lista_global(tabla_primer_nivel, memoria);

	//fclose(archivo_proceso);
}


void hacer_swap_de_pagina_inicio(t_tabla_entradas_primer_nivel* tabla_primer_nivel, t_pagina_segundo_nivel* pagina_iteracion, int id_tabla_segundo_nivel, FILE* archivo_proceso, t_memoria* memoria){
	// Solamente se usa al principio! Esta funcion solo escribe. No se simula RETARDO ya que es una interaccion con kernel y no CPU (issue 2730).

	fseek(archivo_proceso, 0 , SEEK_END);
	int id_pagina = pagina_iteracion->id_pagina;
	void* contenido_de_pagina = malloc(memoria->memoria_config->tamanio_pagina);
	memcpy(contenido_de_pagina, memoria->espacio_memoria, memoria->memoria_config->tamanio_pagina); // No nos importa que escriba, solo que ocupe los bytes que ocupa una pagina.


	fwrite(&contenido_de_pagina, memoria->memoria_config->tamanio_pagina, 1, archivo_proceso);
	fwrite(&id_pagina, sizeof(int), 1, archivo_proceso);
	fwrite(&id_tabla_segundo_nivel, sizeof(int), 1, archivo_proceso);

	free(contenido_de_pagina);
}

void pasar_marco_ocupado_a_marco_libre_global(t_tabla_entradas_primer_nivel* tabla_primer_nivel, t_memoria* memoria){
	int marcos_usados_size = list_size(tabla_primer_nivel->marcos_usados);
	for(int marco_actual = 0; marco_actual < marcos_usados_size; marco_actual++){

		t_marco* marco_iteracion = list_remove(tabla_primer_nivel->marcos_usados, 0);
		list_add(memoria->marcos_libres, marco_iteracion);
	}

}

void agregar_frames_libres_del_proceso_a_lista_global(t_tabla_entradas_primer_nivel* tabla_primer_nivel, t_memoria* memoria){

	t_list* marcos_libres_proceso = tabla_primer_nivel->marcos_libres;
	int marco_actual;
	int marcos_libres_size = list_size(tabla_primer_nivel->marcos_libres);
	for(marco_actual = 0; marco_actual < marcos_libres_size; marco_actual++){
		t_marco* marco_libre = list_remove(marcos_libres_proceso, 0);
		list_add(memoria->marcos_libres, marco_libre);
	}
}


void hacer_reswap_del_proceso(t_pcb* pcb_cliente, t_memoria* memoria){
	// Reserva de frames para el proceso (mover de marcos libres globales a marcos libres del proceso).

	int marcos_por_proceso = memoria->memoria_config->marcos_proceso;
	t_tabla_entradas_primer_nivel* tabla_primer_nivel_del_proceso = obtener_tabla_primer_nivel_del_proceso(pcb_cliente, memoria);

	for(int marco_iteracion = 0; marco_iteracion < marcos_por_proceso; marco_iteracion++){
		t_marco* marco_asignado = list_remove(memoria->marcos_libres, 0);
		list_add(tabla_primer_nivel_del_proceso->marcos_libres, marco_asignado);
	}

}


void sacar_pagina_de_archivo(int pcb_id, t_memoria* memoria, t_marco* marco, t_pagina_segundo_nivel* pagina_a_sacar) {
    //log_info(memoria->memoria_log, "RESWAP PAGINA (page fault): buscando PAGINA %d EN ARCHIVO (proceso %d). Simulando retardo.", pagina_a_sacar->id_pagina, pcb_id);
    //sleep(memoria->memoria_config->retardo_memoria/1000 + memoria->memoria_config->retardo_swap/1000);


    char* path = obtener_path_swap_del_archivo_del_proceso(pcb_id, memoria);

    FILE* archivo_proceso = fopen(path, "rb");
    fseek(archivo_proceso, 0, SEEK_SET);
    int tamanio = tamanio_actual_del_archivo(archivo_proceso);

    void* contenido_archivo = malloc(tamanio);
    fread(contenido_archivo, tamanio, 1, archivo_proceso);

	int offset = 0;
	while(1) {
		int id_pagina;
		int id_tabla_segundo_nivel;
		void* contenido_pagina_iteracion = malloc(memoria->memoria_config->tamanio_pagina);
		memcpy(contenido_pagina_iteracion, contenido_archivo + offset, (memoria->memoria_config->tamanio_pagina));
		offset += memoria->memoria_config->tamanio_pagina;
		memcpy(&id_pagina, contenido_archivo + offset, sizeof(int));
		offset += sizeof(int);
		memcpy(&id_tabla_segundo_nivel, contenido_archivo + offset, sizeof(int));
		offset += sizeof(int);

		if(id_pagina == pagina_a_sacar->id_pagina && id_tabla_segundo_nivel == pagina_a_sacar->tabla_segundo_nivel){
			memcpy(memoria->espacio_memoria + (marco->numero_marco * memoria->memoria_config->tamanio_pagina), contenido_pagina_iteracion, (memoria->memoria_config->tamanio_pagina));

			free(contenido_pagina_iteracion);
			fseek(archivo_proceso, 0, SEEK_SET);
			fclose(archivo_proceso);
			marco->pagina = pagina_a_sacar;
			break;
		}

		free(contenido_pagina_iteracion);
	}

    free(contenido_archivo);
}

void swapear_pagina_en_disco(int pcb_id, t_memoria* memoria, t_marco* marco, t_pagina_segundo_nivel* pagina_a_agregar) {
    //log_info(memoria->memoria_log, "SWAP PAGINA: actualizando PAGINA %d EN ARCHIVO (proceso %d). Simulando retardo.", pagina_a_agregar->id_pagina, pcb_id);
    //sleep(memoria->memoria_config->retardo_memoria/1000 + memoria->memoria_config->retardo_swap/1000);


    char* path_proceso = obtener_path_swap_del_archivo_del_proceso(pcb_id, memoria);
    void* contenido_pagina = malloc(memoria->memoria_config->tamanio_pagina);
    memcpy(contenido_pagina, memoria->espacio_memoria + marco->numero_marco * (memoria->memoria_config->tamanio_pagina), memoria->memoria_config->tamanio_pagina);

    FILE* archivo_proceso = fopen(path_proceso, "rb");
    fseek(archivo_proceso, 0, SEEK_SET);
    int tamanio = tamanio_actual_del_archivo(archivo_proceso);

    void* contenido_archivo = malloc(tamanio);
    fread(contenido_archivo, tamanio, 1, archivo_proceso);
    int tamanio_pagina_con_ids =(memoria->memoria_config->tamanio_pagina) + sizeof(int) + sizeof(int);

	int offset = 0;
    while(1) {
    	int id_pagina;
    	int id_tabla_segundo_nivel;
    	void* contenido_pagina_iteracion = malloc(memoria->memoria_config->tamanio_pagina);
    	memcpy(contenido_pagina_iteracion, contenido_archivo + offset, (memoria->memoria_config->tamanio_pagina));
    	offset+= memoria->memoria_config->tamanio_pagina;
		memcpy(&id_pagina, contenido_archivo + offset, sizeof(int));
		offset += sizeof(int);
		memcpy(&id_tabla_segundo_nivel, contenido_archivo + offset, sizeof(int));
		offset += sizeof(int);

		if(id_pagina == pagina_a_agregar->id_pagina && id_tabla_segundo_nivel == pagina_a_agregar->tabla_segundo_nivel){
			offset -= tamanio_pagina_con_ids;
			memcpy(contenido_archivo + offset , contenido_pagina, (memoria->memoria_config->tamanio_pagina));
			free(contenido_pagina_iteracion);
			fclose(archivo_proceso);
			archivo_proceso = fopen(path_proceso, "wb");
			fwrite(contenido_archivo, tamanio, 1, archivo_proceso);
			fseek(archivo_proceso, 0, SEEK_SET);
			fclose(archivo_proceso);
			break;
		}
		free(contenido_pagina_iteracion);
    }
	free(contenido_archivo);
	free(contenido_pagina);
}

int tamanio_actual_del_archivo(FILE* archivo_proceso) {
    fseek(archivo_proceso, 0, SEEK_END);
    int tamanio = ftell(archivo_proceso);
    fseek(archivo_proceso, 0, SEEK_SET);
    return tamanio;
}

int obtener_espacio_de_memoria_a_acceder(t_memoria* memoria, int frame){
	return (int)(memoria->espacio_memoria) + (frame * memoria->memoria_config->tamanio_pagina);
}
