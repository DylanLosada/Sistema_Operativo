#ifndef CONFIG_CPU_H_
#define CONFIG_CPU_H_

#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<string.h>
#include<unistd.h>
#include<netdb.h>
#include<sys/socket.h>
#include<inttypes.h>
#include<pthread.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<commons/collections/list.h>
#include<commons/collections/queue.h>
#include<commons/error.h>
#include<shared_funtions/estructuras.h>
#include<shared_funtions/conexion_servidor.h>
#include<shared_funtions/conexion_cliente.h>


typedef struct {
	int ENTRADAS_TLB;
	char* REEMPLAZO_TLB;
	int RETARDO_NOOP;
	char* IP_MEMORIA;
	char* PUERTO_MEMORIA;
	char* PUERTO_ESCUCHA_DISPATCH;
	char* PUERTO_ESCUCHA_INTERRUPT;
} t_config_cpu;


typedef struct{
	char* puerto;
	int socket;
	op_code code;
}t_conexion;

typedef struct{
	int socket;
	int size_pagina;
	int cant_entradas_por_tabla;
}t_mem_config;

typedef struct{
	bool is_interrupt;
	pthread_mutex_t* mutex_has_interrupt;
} t_interrupt_message;

typedef struct{
	int socket_cpu;
	t_log* cpu_log;
	t_pcb* pcb;
	op_memoria_message code;
	pthread_mutex_t* mutex_has_io_exit;
}t_args_io_exit;

typedef struct{
	t_log* cpu_log;
	t_config_cpu* cpu_config;
	t_conexion* dispatch;
	t_conexion* interrupt;
	t_mem_config* mem_config;
	t_list* tlb;
	t_interrupt_message* exist_interrupt;
	t_args_io_exit* args_io_exit;
	int last_executed_pcb;
}t_cpu;

t_mem_config* deserialize_handshake(t_cpu* cpu, int socket);

#endif
