#ifndef FUNCIONES_H_
#define FUNCIONES_H_

#include "config_kernel.h"


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


int start_kernel(t_kernel* kernel);
int wait_console(t_kernel* kernel);
void release_connection(int* socket_console);
int recive_operation(int socket_console, t_log* logger);
char* recive_buffer(int socket_console, t_consola* console);


#endif
