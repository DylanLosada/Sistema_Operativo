#include "funciones.h"

void execute_cpu(void* void_args){
	t_cpu* cpu = (t_cpu*) void_args;

	//t_list* listInstructions = list_create();

	cpu->dispatch->socket = wait_kernel(cpu->dispatch->socket);


	/*
	for(int i = 0; i<5; i++){
		list_add(listInstructions, "NO_OP");
	}
	list_add(listInstructions, "I/O 3000");
	list_add(listInstructions, "READ 0");
	list_add(listInstructions, "WRITE 4 42");
	list_add(listInstructions, "COPY 0 4");
	list_add(listInstructions, "EXIT");

	t_pcb* pcb = malloc(sizeof(t_pcb));

	pcb->id = 2;
	pcb->processSize = 50;
	pcb->program_counter = 0;
	pcb->tabla_paginas = 4;
	pcb->rafaga = 10000;
	pcb->time_io = 0;
	pcb->time_excecuted_rafaga = 0;
	pcb->time_blocked = 0;
	pcb->instrucciones = listInstructions;
	*/

	//cpu->dispatch->socket = 1;

	int* code;
	t_pcb* pcb = deserializate_pcb(cpu->dispatch->socket, code);

	if(*code < 0){
		//TODO
	}

	log_info(cpu->cpu_log,"KERNEL CONECTADO A PUERTO %s",cpu->dispatch->puerto);
	fetch_and_decode(pcb, cpu);

}


int start_cpu(char* puerto, t_log* logger){
	return create_server_connection(puerto, logger, "CPU LISTA PARA RECIBIR INSTRUCCIONES");
}


int wait_kernel(int socket){
	// Aceptamos un nuevo cliente
	int socket_kernel = accept(socket, NULL, NULL);

	return socket_kernel;
}


