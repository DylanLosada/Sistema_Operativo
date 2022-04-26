#ifndef CONSOLA_H_
#define CONSOLA_H_

#include<stdio.h>
#include<stdlib.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<readline/readline.h>

t_log* iniciar_logger(void);
t_config* iniciar_config(void);
char* serializeInstruction(int ,char*);
void leer_consola(t_log*);
void checkCodeOperation(char*, char**);
char* generateInstructiosnString(char*);
void appendOperationToInstructionsString(char*, char**);
void appendNoOpToInstructionsString(char**,char**);

#endif /*CONSOLA_H_*/
