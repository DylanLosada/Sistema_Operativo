#include "cpu.h"



int main() {

	t_log* logger = log_create("cpu.log", "CPU_MAIN", 1, LOG_LEVEL_DEBUG);
	t_config* config = iniciar_config();
	int no_op_time = config_get_int_value(config, "RETARDO_NOOP");




	// Handshake con memoria para conseguir config cantidad entradas por tabla de paginas y tamaño de pagina.
	char* ip_memoria = config_get_string_value(config, "IP_MEMORIA");
	char* puerto_memoria = config_get_string_value(config, "PUERTO_MEMORIA");
	// TODO: connection



	// Conexion puerto dispatch kernel para recibir pcb a ejecutar.
	char* puerto_escucha_dispatch = config_get_string_value(config, "PUERTO_ESCUCHA_DISPATCH");
	// TODO: connection




	// Handle pcb (fetch, decode, fetch operands, execute, check interrupt)
	t_pcb* pcb = malloc(sizeof(t_pcb));
	pcb->id = 1;
	pcb->tamanio = 10;
	pcb->instrucciones = list_create();


	// Test instructions:
	t_instruct* instruct = malloc(sizeof(t_instruct));
	instruct->op_code = NO_OP;
	instruct->param1 = 1;
	instruct->param2 = 2;


	t_link_element* head = malloc(sizeof(t_link_element));
	head->data = instruct;


	pcb->instrucciones->head = head;
	pcb->pc = 0;
	//pcb.tabla_paginas????
	pcb->rafaga = 0;

	t_instruct* current_instruct = malloc(sizeof(t_instruct));
	fetch_and_decode(pcb, current_instruct);
	execute(current_instruct, no_op_time / 1000);


	//list_destroy(pcb->instrucciones);
	free(pcb);




	// Conexion puerto interrupt kernel.
	char* puerto_escucha_interrupt = config_get_string_value(config, "PUERTO_ESCUCHA_INTERRUPT");




	// Handle interrupt.





   return 0;
}


t_config* iniciar_config(void){
	return config_create("cpu.config");
}
