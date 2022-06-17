#ifndef CONEXION_H_
#define CONEXION_H_

#include "funciones.h"
#include <shared_funtions/free_memory.h>
#include "pcb.h"

typedef struct {
    t_kernel* kernel;
    int fd;
    char* server_name;
    pthread_mutex_t* semaforo;
    pthread_mutex_t* hasNewConsole;
    t_queue* cola_pre_pcb;
} t_process_conexion;

void create_thread(t_queue* cola_pre_pcb, char* algoritmo_planificacion);

#endif
