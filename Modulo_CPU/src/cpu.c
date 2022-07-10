#include "cpu.h"

int main() {

	t_cpu* cpu = malloc(sizeof(t_cpu));
	t_log* cpu_logger = log_create("cpu.log", "CPU_MAIN", 1, LOG_LEVEL_DEBUG);

	cpu->cpu_log = cpu_logger;
	cpu->cpu_config = create_config_cpu(cpu_logger);
	cpu->tlb = list_create();
	cpu->last_executed_pcb = -1;
	wait_handshake(cpu, cpu->cpu_config->PUERTO_MEMORIA, cpu->cpu_config->IP_MEMORIA);

	pthread_mutex_t* mutex_io_exit = malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(mutex_io_exit, NULL);
	pthread_mutex_lock(mutex_io_exit);

	pthread_t hilo_dispatch;
	t_conexion* dispatch = malloc(sizeof(t_conexion));

	dispatch->puerto = cpu->cpu_config->PUERTO_ESCUCHA_DISPATCH;
	dispatch->code = 1;
	dispatch->socket = start_cpu(dispatch->puerto, cpu->cpu_log, "dispatch");

	cpu->dispatch = dispatch;

	pthread_mutex_t* mutex_has_interrupt = malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(mutex_has_interrupt, NULL);

	pthread_t hilo_interrupt;
	pthread_t hilo_blocked_exit;

	t_conexion* interrupt = malloc(sizeof(t_conexion));

	interrupt->puerto = cpu->cpu_config->PUERTO_ESCUCHA_INTERRUPT;
	interrupt->code = 2;
	interrupt->socket = start_cpu(interrupt->puerto, cpu->cpu_log, "interrupt");
	cpu->interrupt = interrupt;

	t_interrupt_message* exist_interrupt = malloc(sizeof(t_interrupt_message));

	exist_interrupt->mutex_has_interrupt = mutex_has_interrupt;
	exist_interrupt->is_interrupt = false;

	cpu->exist_interrupt = exist_interrupt;

	t_args_io_exit* args_io_exit = malloc(sizeof(t_args_io_exit));
	args_io_exit->mutex_has_io_exit = mutex_io_exit;
	args_io_exit->socket_cpu = start_cpu("9000", cpu->cpu_log, "exit/blocked");
	args_io_exit->cpu_log = cpu_logger;
	args_io_exit->cpu = cpu;

	cpu->args_io_exit = args_io_exit;

	pthread_create(&hilo_dispatch, NULL, (void*)execute_dispatch, (void*)cpu);
	pthread_create(&hilo_interrupt, NULL, (void*)execute_interrupt, (void*)cpu);
	pthread_create(&hilo_blocked_exit, NULL, (void*)execute_blocked_exit, (void*)args_io_exit);

	pthread_detach(hilo_dispatch);
	pthread_detach(hilo_interrupt);
	pthread_join(hilo_blocked_exit, NULL);


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
	sleep(2);
	void* a_enviar = malloc(sizeof(int));
	memcpy(a_enviar, &op_code, sizeof(int));
	send_data_to_server(socket, a_enviar, sizeof(int));
	log_info(cpu->cpu_log, "HANDSHAKE REALIZADO, ESPERAMOS DATOS.");
	recv(socket, &handshake->size_pagina, sizeof(int), MSG_WAITALL);
	recv(socket, &handshake->cant_entradas_por_tabla, sizeof(int), MSG_WAITALL);
	log_info(cpu->cpu_log, "DATOS RECIBIDOS.");
	return handshake;
}

