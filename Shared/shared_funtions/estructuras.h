/*
 * estructuras.h
 *
 *  Created on: Apr 30, 2022
 *      Author: dlosada
 */

#ifndef SHARED_FUNTIONS_ESTRUCTURAS_H_
#define SHARED_FUNTIONS_ESTRUCTURAS_H_
#include <commons/collections/list.h>

typedef struct{
	int processSize;
	int streamLength;
	void* stream;
} t_consola;

typedef enum
{
	NO_OP,
	I_O,
	READ,
	COPY,
	WRITE,
	EXIT
} op_instructions_code;

typedef struct t_pcb{
	int id;
	int processSize;
	int program_counter;
	int tabla_paginas;
	int rafaga;
	int time_io;
	int time_excecuted_rafaga;
	clock_t time_blocked;
	t_list* instrucciones;
} t_pcb;

typedef enum {
    CONSOLA,
	DISPATCH,
	INTERRUPT
} op_code;

#endif /* SHARED_FUNTIONS_ESTRUCTURAS_H_ */
