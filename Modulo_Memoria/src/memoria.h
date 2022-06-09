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



//Funciones que sirven para memoria tales como estructuras o funciones

/* These values may be used as exit status codes.
 */
#define MEMORIA_PRINCIPAL 0
#define MEMORIA_VIRTUAL 1

#define CLOCL-M 1
#define CLOCK 2

typedef enum{
	MENSAJE,
	PAQUETE,
	ELIMINAR_PROCESO,
	PROCESO_ELIMINADO,
	HACER_SWAP,
	SWAP_HECHO,
	//paginacion??
    OPERACION_EXITOSA,
    OPERACION_FALLIDA,
}op_code;

typedef struct{
    int size;
    void* stream;
} t_buffer;

typedef struct{
    op_code codigo_operacion;
    t_buffer* buffer;
} t_paquete;

typedef struct{
	char * puerto;
	char * ip_memoria;
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
    int id_pagina;
    int frame_principal;
    int frame_virtual;
    int presencia;
    int uso;
    int modificado;
    int lock;
    int tiempoDeUso;
    int tamanioDisponible;
    int fragmInterna;
}t_pagina;

typedef struct{
    int id_proceso;
    int tamanio_instrucciones;
    int direccionPCB;
    t_list* paginas;
}t_tabla_pagina;

t_config_memoria config_memoria;

char * memoria_principal;

//**************************FUNCIONES********************+
int iniciar_memoria(void);
void manejar_conexion(int server_fd);
int administrar_cliente(int cliente_fd);
void iniciar_proceso(int cliente_fd);
int guardar_proceso_en_paginacion(int idProceso, char * instrucciones, int tamanio_proceso);
t_list* guardar_proceso(int idProceso, int tamanio_proceso);
t_list* guardar_proceso_en_paginas(int idProceso, int tamanio);


//////////////////////////////SEMAFOROS////////////////////////////

pthread_mutex_t mutex_memoria_principal_bitmap;


#endif
