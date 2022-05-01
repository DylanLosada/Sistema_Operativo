#ifndef CONEXION_H_
#define CONEXION_H_

#include "funciones.h"
#include "pcb.h"

typedef struct {
    t_kernel* kernel;
    int fd;
    char* server_name;
    pthread_mutex_t* semaforo;
    t_queue* cola_pre_pcb;
} t_process_conexion;

void create_thread(t_queue* cola_pre_pcb, char* algoritmo_planificacion);

#endif
