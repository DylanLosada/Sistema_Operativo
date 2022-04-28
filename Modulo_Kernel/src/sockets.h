#ifndef SOCKETS_H_
#define SOCKETS_H_

#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<unistd.h>
#include<netdb.h>
#include<commons/log.h>
#include<string.h>
#include<commons/collections/list.h>
#include<commons/config.h>

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
	int processSize;
	int streamLength;
	void* stream;
} t_consola;

typedef struct{
	int op_code;
	t_consola* consola;
} t_paquete;


typedef struct{
	char* op_code;
	int* firstParam;
	int* secondParam;
} __attribute__((packed))
t_instructions;

int start_kernel(char* puerto, t_log* kernel_logger);
int wait_console(int socket_kernel, t_log* kernel_logger);
void release_connection(int* socket_console);
int recibir_operacion(int socket_cliente, t_log* logger);
char* recibir_buffer(int, t_consola*);



#endif
