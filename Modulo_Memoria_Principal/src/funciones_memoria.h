#ifndef UTILS_H_
#define UTILS_H_

#include<commons/config.h>
#include<commons/log.h>
#include<commons/collections/list.h>
#include <shared_funtions/conexion_servidor.h>
#include <dirent.h>
#include<string.h>

typedef enum{
	CLOCK,
	CLOCK_MODIFICADO
} t_algoritmo_reemplazo_mem;

typedef struct{
	char* puerto;
	int tamanio_memoria;
	int tamanio_pagina;
	int entradas_por_tabla;
	int retardo_memoria;
	t_algoritmo_reemplazo_mem algoritmo_reemplazo;
	int marcos_proceso;
	int retardo_swap;
	char * path_swap;
}t_config_memoria;

typedef struct{
	t_log* memoria_log;
	t_config_memoria* memoria_config;
	int server_fd;
	t_list* tablas_primer_nivel;
	t_list* tablas_segundo_nivel;
	int id_tablas_primer_nivel;
	int id_tablas_segundo_nivel;
	void* espacio_memoria;
	t_list* marcos_libres;
}t_memoria;


#endif

