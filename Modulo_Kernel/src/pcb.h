#ifndef PCB_H_
#define PCB_H_

#include "kernel.h";
#include "sockets.h";


typedef struct t_pcb{
	int id;
	int tamanio;
	t_list instrucciones;
	int pc;
	void* tabla_paginas;
	int rafaga;
}t_pcb;


#endif /* PCB_H_ */
