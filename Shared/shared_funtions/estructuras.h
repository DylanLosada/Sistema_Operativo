#ifndef SHARED_FUNTIONS_ESTRUCTURAS_H_
#define SHARED_FUNTIONS_ESTRUCTURAS_H_
#include <commons/collections/list.h>
#include <commons/log.h>
#include <commons/string.h>
#include<sys/socket.h>

typedef struct{
	int processSize;
	int streamLength;
	void* stream;
} t_consola;

typedef enum
{
	NO_OP,
	I_O,
	READ,
	COPY,
	WRITE,
	EXIT
} op_instructions_code;

typedef enum{
	ERROR,
	SWAP,
	RE_SWAP,
	NEW,
	DELETE
} op_memoria_message;

typedef struct{
	int id;
	int processSize;
	int program_counter;
	int* tabla_paginas;
	int rafaga;
	int time_io;
	int time_excecuted_rafaga;
	clock_t time_blocked;
	t_list* instrucciones;
} t_pcb;

typedef enum {
    CONSOLA,
	HANDSAKE,
	DISPATCH,
	INTERRUPT,
	BLOCKED,
	FINISHED,
	IO
} op_code;

typedef struct {
	int size;
	void* stream;
}t_buffer;

typedef struct{
	int op_code;
	t_buffer* buffer;
} t_cpu_paquete;

typedef struct{
	int op_code;
	int tamanio_pag;
	int entrada_por_tabla;
} t_handshake_paquete;

void loggear_pcb(t_pcb* pcb);
t_pcb* deserializate_pcb(int socket, int* op_code);
void* serializate_pcb(t_pcb* pcb, t_cpu_paquete* paquete, int MENSSAGE);

#endif /* SHARED_FUNTIONS_ESTRUCTURAS_H_ */
