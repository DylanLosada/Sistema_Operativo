#ifndef CONSOLA_H_
#define CONSOLA_H_

#include<stdio.h>
#include<stdlib.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<readline/readline.h>

typedef struct{
	char* op_code;
	int* firstParam;
	int* secondParam;
} __attribute__((packed))
t_serializable;

t_log* iniciar_logger(void);
t_config* iniciar_config(void);
char* serializeInstruction(int ,char*);
void leer_consola(t_log*);
void paquete(int);

#endif /*CONSOLA_H_*/
