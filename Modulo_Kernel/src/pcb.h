#ifndef PCB_H_
#define PCB_H_

#include "commons/collections/list.h"

typedef struct {
    int processSize;
    t_list* instructions;
    int pcb_id;
} t_pre_pcb;
/*
typedef struct{
	int id;
	int processSize;
	t_list instrucciones;
	int* program_counter;
	int* tabla_paginas;
	int* rafaga;
	int time_io;
	clock_t time_blocked;
	int time_excecuted_rafaga;
}t_pcb; */

#endif /* PCB_H_ */
