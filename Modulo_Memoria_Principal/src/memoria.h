#ifndef MEMORIA_H_
#define MEMORIA_H_

#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include<commons/log.h>
#include<unistd.h>
#include<netdb.h>
#include<commons/collections/list.h>
#include<commons/collections/queue.h>
#include<commons/config.h>
#include<dirent.h>
#include<errno.h>
#include<shared_funtions/estructuras.h>
#include<math.h>



t_log* logger;
char* memoriaRAM;

typedef struct{
	char* puerto;
	int tamanio_memoria;
	int tamanio_pagina;
	int entradas_por_tabla;
	int retardo_memoria;
	char * algoritmo_reemplazo;
	int marcos_proceso;
	int retardo_swap;
	char * path_swap;
}t_config_memoria;

typedef struct{
	t_log* memoria_log;
	t_config_memoria* memoria_config;
	int server_fd;
	t_list* tablas_primer_nivel;
	t_list* tablas_segundo_nivel;
}t_memoria;

t_memoria* memoria;

typedef struct{
    op_code codigo_operacion;
    t_buffer* buffer;
} t_paquete;

typedef struct{
    int socket;
    pthread_mutex_t* semaforo_conexion;
} t_args_administrar_cliente;

//********************PAGINACION***************************
typedef struct{
	int id_proceso;
    int id_tabla;
    t_list* entradas;
}t_tabla_paginas_primer_nivel;

typedef struct{
    int id_pagina;
    int frame_principal;
    int presencia;
    int uso;
    int modificado;
}t_pagina_segundo_nivel;

typedef struct{
	int id_tabla;
	t_list* paginas_segundo_nivel;
}t_tabla_paginas_segundo_nivel;

//**************************FUNCIONES********************+
int* iniciar_memoria_paginada();
void manejar_conexion(void* void_args);
int administrar_cliente(t_args_administrar_cliente* args_administrar_cliente);
void iniciar_proceso(t_pcb* pcb_cliente, int cliente_fd);
int guardar_proceso_en_paginacion(t_pcb* pcb_cliente);
void hacer_handshake_con_cpu(int cliente_fd);
t_list* guardar_proceso(int idProceso, int tamanio_proceso);
t_list* guardar_proceso_en_paginas(int idProceso, int tamanio);


//////////////////////////////SEMAFOROS////////////////////////////

pthread_mutex_t mutex_memoria_principal_bitmap;

#endif
