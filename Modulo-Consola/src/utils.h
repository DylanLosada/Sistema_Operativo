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

typedef enum
{
	MENSAJE,
	PAQUETE
}op_code;

typedef struct
{
	int size;
	void* stream;
} t_buffer;

typedef struct
{
	int processSize;
	t_buffer* buffer;
} t_paquete;



int crear_conexion(char* ip, char* puerto);
void enviar_mensaje(char*, int , int);
t_paquete* crear_paquete(int);
t_paquete* crear_super_paquete(int);
void agregar_a_paquete(t_paquete* paquete, void* valor, int tamanio);
void enviar_paquete(t_paquete* paquete, int socket_cliente);
void liberar_conexion(int socket_cliente);
void* serializar_paquete(t_paquete* , int);
void eliminar_paquete(t_paquete* paquete);

#endif /* UTILS_H_ */
