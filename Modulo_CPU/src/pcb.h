#ifndef SRC_PCB_H_
#define SRC_PCB_H_

#include <commons/collections/list.h>


typedef struct {
	int id;
	int tamanio;
	t_list* instrucciones;
	int pc;
	void* tabla_paginas;
	int rafaga;
} t_pcb;


typedef enum
{
	NO_OP,
	I_O,
	READ,
	COPY,
	WRITE,
	EXIT
} op_code;


typedef struct {
	op_code op_code;
	int param1;
	int param2;
} t_instruct;


#endif /* SRC_PCB_H_ */
