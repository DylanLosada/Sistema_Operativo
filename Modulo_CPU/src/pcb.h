#ifndef SRC_PCB_H_
#define SRC_PCB_H_

typedef struct t_pcb{
	int id;
	int tamanio;
	t_list instrucciones;
	int pc;
	void* tabla_paginas;
	int rafaga;
}t_pcb;

#endif /* SRC_PCB_H_ */
