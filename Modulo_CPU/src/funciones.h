#ifndef FUNCIONES_H_
#define FUNCIONES_H_

#include "pcb.h"
#include "tlb.h"

void execute_interrupt(void* void_args);
void execute_dispatch(void* void_args);
void execute_blocked_exit(void* void_args);
int start_cpu(char* puerto, t_log* logger, char* conexion);
int wait_kernel(t_log* log, int socket, char* puerto);

#endif
