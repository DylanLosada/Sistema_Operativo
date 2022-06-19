#include "cpu.h"

int main() {

	t_cpu* cpu = malloc(sizeof(t_cpu));
	t_log* cpu_logger = log_create("cpu.log", "CPU_MAIN", 1, LOG_LEVEL_DEBUG);

	cpu->cpu_log = cpu_logger;
	cpu->cpu_config = create_config_cpu(cpu_logger);
	cpu->tlb = list_create();
	wait_handshake(cpu, cpu->cpu_config->PUERTO_MEMORIA, cpu->cpu_config->IP_MEMORIA);

	pthread_t hilo_dispatch;
	t_conexion* dispatch = malloc(sizeof(t_conexion));

	dispatch->puerto = cpu->cpu_config->PUERTO_ESCUCHA_DISPATCH;
	dispatch->code = 1;
	dispatch->socket = start_cpu(dispatch->puerto, cpu->cpu_log, "dispatch");

	cpu->dispatch = dispatch;

	pthread_mutex_t* mutex_has_interrupt = malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(mutex_has_interrupt, NULL);

	pthread_t hilo_interrupt;
	t_conexion* interrupt = malloc(sizeof(t_conexion));

	interrupt->puerto = cpu->cpu_config->PUERTO_ESCUCHA_INTERRUPT;
	interrupt->code = 2;
	interrupt->socket = start_cpu(interrupt->puerto, cpu->cpu_log, "interrupt");
	cpu->interrupt = interrupt;

	t_interrupt_message* exist_interrupt = malloc(sizeof(t_interrupt_message));

	exist_interrupt->mutex_has_interrupt = mutex_has_interrupt;
	exist_interrupt->is_interrupt = false;

	cpu->exist_interrupt = exist_interrupt;

	pthread_create(&hilo_dispatch, NULL, (void*)execute_dispatch, (void*)cpu);
	pthread_create(&hilo_interrupt, NULL, (void*)execute_interrupt, (void*)cpu);

	while(1);

	pthread_detach(hilo_dispatch);
	pthread_detach(hilo_interrupt);

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

void wait_handshake(t_cpu* cpu, char* puerto, char* ip){
	int socket = create_client_connection(ip, puerto);
	cpu->mem_config = deserialize_handshake(cpu, socket);
}

t_mem_config* deserialize_handshake(t_cpu* cpu, int socket){
	t_mem_config* handshake = malloc(sizeof(t_mem_config));
	handshake->socket = socket;
	int op_code = HANDSHAKE;
	log_info(cpu->cpu_log, "CONECTADO A MEMORIA, ESPERAMOS HANDSHAKE.");
	send_data_to_server(socket, &op_code, sizeof(int), 0);
	log_info(cpu->cpu_log, "HANDSHAKE REALIZADO, ESPERAMOS DATOS.");
	recv(socket, &handshake->size_pagina, sizeof(int), MSG_WAITALL);
	recv(socket, &handshake->cant_entradas_por_tabla, sizeof(int), MSG_WAITALL);
	log_info(cpu->cpu_log, "DATOS RECIBIDOS.");
	return handshake;
}

