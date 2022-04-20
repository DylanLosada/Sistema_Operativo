#ifndef CONSOLA_H_
#define CONSOLA_H_

#include<stdio.h>
#include<stdlib.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<readline/readline.h>

#include "utils.h"

typedef struct{
	char* op_code;
	int* firstParam;
	int* secondParam;
} __attribute__((packed))
t_serializable;

t_log* iniciar_logger(void);
t_config* iniciar_config(void);
void leer_consola(t_log*);
void paquete(int);
void terminar_programa(int, t_log*, t_config*);
t_paquete* makePaquete(int, char*, int);
void generateInstructs(char* , char**);
void checkCodeOperation(char* ,char**);

#endif /*CONSOLA_H_*/
