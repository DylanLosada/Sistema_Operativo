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

//#include<assert.h>

/*
t_log* logger;

void* recibir_buffer(int*, int);
*/
typedef struct{
    int size;
    void* stream;
} t_buffer;

typedef struct{
    int processSize;
    t_buffer* buffer;
} t_paquete;

typedef struct{
	char* op_code;
	int* firstParam;
	int* secondParam;
} __attribute__((packed))
t_instructions;

int start_kernel(t_log* logger, const char* name, char* puerto);
int wait_console(t_log* logger, const char* name, int socket_kernel);
void release_connection(int* socket_console);
void recibir_operacion(int socket_cliente, t_paquete* paquete, t_log* logger);
char* recibir_buffer(int socket_cliente, t_paquete* paquete);



#endif
