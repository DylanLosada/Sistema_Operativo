#ifndef MEMORIA_H_
#define MEMORIA_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "conexion_servidor.h"
#include <stdbool.h>

typedef struct{
	int puerto;
	int tamanio_memoria;
	int tamanio_pagina;
	int entradas_por_tabla;
	int retardo_memoria;
	char * algoritmo_reemplazo;
	int marcos_proceso;
	int retardo_swap;
	char * path;
}t_config_memoria;

t_config_memoria config_memoria;

char * memoria_principal;
bool funcionando;


#endif
