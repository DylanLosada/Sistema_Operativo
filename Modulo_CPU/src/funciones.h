#ifndef FUNCIONES_H_
#define FUNCIONES_H_

#include "pcb.h"
#include "check_interrupt.h"

void execute_interrupt(void* void_args);
void execute_dispatch(void* void_args);
int start_cpu(char* puerto, t_log* logger);


#endif
