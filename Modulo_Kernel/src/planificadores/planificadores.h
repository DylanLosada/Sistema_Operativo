#ifndef PLANIFICADORES_H_
#define PLANIFICADORES_H_

#include <commons/collections/queue.h>
#include <pthread.h>

typedef enum{
	FIFO,
	SRT
} tipo_planificador_enum;

typedef struct{
	t_queue* pre_pcbs;
	int multiprogramming_degree;
	char* planner_type;
	pthread_mutex_t* mutex;
}t_args_planificador;

void planificador_largo_plazo(char* tipo_planificador, t_queue* cola_nuevo, t_queue* cola_salidas);
void ordenar_cola(char* tipo_planificador, t_queue* cola_a_ordenar);
void handler_planners(void* void_args);

#endif;
