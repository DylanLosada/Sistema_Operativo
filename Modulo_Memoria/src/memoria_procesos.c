#include "memoria.h"
#include "utils.h"

int iniciar_memoria(void){
	char* path_config_memoria = "/home/utnso/tp-2022-1c-SanguchitOS-main/Modulo_Memoria/memoria.config";
	t_config* config = leer_config(path_config_memoria);
	config_memoria.tamanio_memoria = config_get_int_value(config, "TAM_MEMORIA");
	config_memoria.tamanio_pagina = config_get_int_value(config, "TAM_PAGINA");
	config_memoria.entradas_por_tabla = config_get_int_value(config, "ENTRADAS_POR_TABLA");
	config_memoria.retardo_memoria = config_get_int_value(config, "RETARDO_MEMORIA");
	config_memoria.algoritmo_reemplazo = config_get_string_value(config, "ALGORITMO_REEMPLAZO");
	config_memoria.marcos_proceso = config_get_int_value(config, "MARCOS_POR_PROCESO");
	config_memoria.retardo_swap = config_get_int_value(config, "RETARDO_SWAP");
	config_memoria.path = config_get_string_value(config, "PATH_SWAP");

	memoria_principal = malloc(config_memoria.tamanio_memoria);

	if(memoria_principal == NULL){
		perror("MALLOC FAIL!\n");
		return 0;
	}

	int tamanio_paginas = config_memoria.tamanio_pagina;



	return 0;
}
