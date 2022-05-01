#ifndef CONEXION_H_
#define CONEXION_H_

#include "funciones.h"
#include "protocolo.h"

typedef struct {
    t_kernel* kernel;
    int fd;
    char* server_name;
    pthread_mutex_t* semaforo;
    t_queue* cola_pre_pcb;
} t_process_conexion;

typedef struct {
    int processSize;
    t_list* instructions;
    int ppid;
} t_pre_pcb;

int server_escuchar(t_log* logger, char* server_name, int server_socket);
void create_thread(t_queue* cola_pre_pcb, char* algoritmo_planificacion);

#endif
