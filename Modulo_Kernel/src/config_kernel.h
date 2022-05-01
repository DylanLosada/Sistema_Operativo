#ifndef CONFIG_KERNEL_H_
#define CONFIG_KERNEL_H_

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
#include<commons/config.h>
#include<commons/collections/list.h>
#include<commons/collections/queue.h>
#include "shared_funtions/estructuras.h"

typedef struct {
	char* IP_MEMORIA;
	char* PUERTO_MEMORIA;
	char* IP_CPU;
	char* PUERTO_CPU_DISPATCH;
	char* PUERTO_CPU_INTERRUPT;
	char* PUERTO_ESCUCHA;
	char* ALGORITMO_PLANIFICACION;
	char* ESTIMACION_INICIAL;
	char* ALFA;
	char* GRADO_MULTIPROGRAMACION;
	char* TIEMPO_MAXIMO_BLOQUEADO;
} t_config_kernel;


typedef struct{
	t_log* kernel_log;
	t_config_kernel* kernel_config;
	char* nombre;
	int kernel_socket;
}t_kernel;

t_config_kernel* create_config(t_log* log);

#endif
