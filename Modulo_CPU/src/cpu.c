#include "cpu.h"

int main() {


	t_cpu* cpu = malloc(sizeof(t_cpu));
	t_log* cpu_logger = log_create("cpu.log", "CPU_MAIN", 1, LOG_LEVEL_DEBUG);
	cpu->cpu_log = cpu_logger;
	cpu->cpu_config = create_config_cpu(cpu_logger);

	pthread_t hilo_dispatch;
	t_info* dispatch = malloc(sizeof(t_info));

	dispatch->puerto = cpu->cpu_config->PUERTO_ESCUCHA_DISPATCH;
	dispatch->code = 1;
	dispatch->socket = start_cpu(dispatch->puerto, cpu_logger);
	dispatch->log = cpu_logger;

	pthread_create(&hilo_dispatch, NULL, (void*)execute_cpu, (void*)dispatch);


	pthread_t hilo_interrupt;
	t_info* interrupt = malloc(sizeof(t_info));

	interrupt->puerto = cpu->cpu_config->PUERTO_ESCUCHA_INTERRUPT;
	interrupt->code = 2;
	interrupt->socket = start_cpu(interrupt->puerto);
	interrupt->log = cpu_logger;

	pthread_create(&hilo_interrupt, NULL, (void*)execute_cpu, (void*)interrupt);

	while(1);

	pthread_detach(hilo_dispatch);
	pthread_detach(hilo_interrupt);
/*
	pthread_t hilo_interrupt;

	t_info* args_interrupt = malloc(sizeof(t_info));

	args_interrupt->puerto = cpu->cpu_config->PUERTO_ESCUCHA_INTERRUPT;
	args_interrupt->texto = "CPU INTERRUPT a la espera de mensajes de interrupcion..";
	args_interrupt->logger = cpu_logger;

	pthread_create(&hilo_interrupt, NULL, execute_cpu , args_interrupt);
	pthread_detach(hilo_interrupt);
*/
/*
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




*/
   return 0;
}

