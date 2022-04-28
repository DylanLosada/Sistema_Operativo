#include "cpu.h"



int main() {

	t_log* logger = log_create("kernel.log", "Kernel", 1, LOG_LEVEL_DEBUG);
	t_config* config = iniciar_config();




	// Handshake con memoria para conseguir config cantidad entradas por tabla de paginas y tamaño de pagina.
	char* ip_memoria = config_get_string_value(config, "IP_MEMORIA");
	char* puerto_memoria = config_get_string_value(config, "PUERTO_MEMORIA");
	// TODO: connection



	// Conexion puerto dispatch kernel para recibir pcb a ejecutar.
	char* puerto_escucha_dispatch = config_get_string_value(config, "PUERTO_ESCUCHA_DISPATCH");
	// TODO: connection




	// Handle pcb (decode, fetch operands, execute, check interrupt)
	t_pcb* pcb = malloc(sizeof(t_pcb));
	pcb->id = 1;
	pcb->tamanio = 10;
	pcb->instrucciones = create_list();

	t_link_element head;
	// TODO: test instructions

	pcb->instrucciones->head = head;
	pcb->pc = 0;
	//pcb.tabla_paginas????
	pcb->rafaga = 0;

	fetch(pcb);

	list_destroy(pcb->instrucciones);
	free(pcb);




	// Conexion puerto interrupt kernel.
	char* puerto_escucha_interrupt = config_get_string_value(config, "PUERTO_ESCUCHA_INTERRUPT");




	// Handle interrupt.





   return 0;
}


t_config* iniciar_config(void){
	return config_create("cpu.config");
}
