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
#include<estructuras.h>


typedef struct {
	char* ENTRADAS_TLB;
	char* REEMPLAZO_TLB;
	char* RETARDO_NOOP;
	char* IP_MEMORIA;
	char* PUERTO_MEMORIA;
	char* PUERTO_ESCUCHA_DISPATCH;
	char* PUERTO_ESCUCHA_INTERRUPT;
} t_config_cpu;


typedef struct{
	char* puerto;
	int socket;
	op_code code;
	t_log* log;
}t_info;


typedef struct{
	t_log* cpu_log;
	t_config_cpu* cpu_config;
}t_cpu;


#endif
