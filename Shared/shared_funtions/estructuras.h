#ifndef SHARED_FUNTIONS_ESTRUCTURAS_H_
#define SHARED_FUNTIONS_ESTRUCTURAS_H_
#include <commons/collections/list.h>
#include <stdlib.h>
#include <string.h>
#include <commons/log.h>
#include <commons/string.h>
#include<sys/socket.h>

typedef struct{
	int processSize;
	int streamLength;
	void* stream;
} t_consola;

/*typedef enum
{
	NO_OP,
	I_O,
	READ,
	COPY,
	WRITE,
	EXIT
} op_instructions_code;*/

typedef enum{
	ERROR,
	SWAP,
	RE_SWAP,
	NEW,
	HANDSHAKE,
	OPERACION_EXITOSA,
	OPERACION_FALLIDA,
	DELETE,
	TABLA_SEGUNDO_NIVEL,
	MARCO,
	OK,
	NO_OP,
	I_O,
	READ,
	COPY,
	WRITE,
	EXIT
} op_memoria_message;

typedef struct{
    int tabla_nivel;
    int entrada_nivel;
    op_memoria_message instruccion;
} t_administrar_mmu;

typedef struct{
	int id;
	int processSize;
	int program_counter;
	int tabla_paginas;
	int rafaga;
	int time_io;
	int time_excecuted_rafaga;
	clock_t time_blocked;
	t_list* instrucciones;
} t_pcb;

typedef enum {
    CONSOLA,
	DISPATCH,
	INTERRUPT,
	BLOCKED_FINISHED
} op_code;

typedef struct {
	int size;
	void* stream;
}t_buffer;

typedef struct{
	int op_code;
	t_buffer* buffer;
} t_cpu_paquete;

void loggear_pcb(t_pcb* pcb);
t_pcb* deserializate_pcb(int socket, int* op_code);
t_pcb* deserializate_pcb_memoria(int socket);
void deserialize_mmu_memoria(t_administrar_mmu* administrar_mmu, int socket);
void* serialize_mmu_memoria(t_cpu_paquete* paquete, int tabla_nivel, int entrada_nivel, int MENSSAGE);
void* serializate_pcb(t_pcb* pcb, t_cpu_paquete* paquete, int MENSSAGE);

#endif /* SHARED_FUNTIONS_ESTRUCTURAS_H_ */
