#ifndef UTILS_H_
#define UTILS_H_

#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include<commons/log.h>
#include <estructuras.h>

typedef struct
{
	int op_code;
	t_consola* consola;
} t_paquete;

int crear_conexion(char* ip, char* puerto);
void send_instructions(char*, int , int);
void liberar_conexion(int socket_cliente);
void* serializate_instructions(char*, int, t_paquete*);
void eliminar_paquete(t_paquete* paquete);

#endif /* UTILS_H_ */
