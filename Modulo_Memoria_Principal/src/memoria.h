#ifndef MEMORIA_H_
#define MEMORIA_H_

#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netdb.h>
#include <commons/error.h>
#include "funciones_memoria.h"
#include<unistd.h>
#include<netdb.h>
#include<commons/collections/queue.h>
#include<dirent.h>
#include<errno.h>
#include<shared_funtions/estructuras.h>
#include<math.h>

typedef struct{
    op_code codigo_operacion;
    t_buffer* buffer;
} t_paquete;

typedef struct{
    int socket;
    t_memoria* memoria;
    pthread_mutex_t* semaforo_conexion;
} t_args_administrar_cliente;

//********************PAGINACION***************************
typedef struct{
	int id_proceso;
    int id_tabla;
    t_list* entradas;
}t_tabla_entradas_primer_nivel;

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
void manejar_conexion(void* void_args);
int administrar_cliente(t_args_administrar_cliente* args_administrar_cliente);
void iniciar_proceso(t_pcb* pcb_cliente, int cliente_fd, t_memoria* memoria);
t_pcb* guardar_proceso_en_paginacion(t_pcb* pcb_cliente, t_memoria* memoria);
void hacer_handshake_con_cpu(int cliente_fd, t_memoria* memoria);
t_list* guardar_proceso(int idProceso, int tamanio_proceso);
t_list* guardar_proceso_en_paginas(int idProceso, int tamanio);
int start_memoria(t_memoria* memoria);
t_config_memoria* create_config(t_log* log);
void aumentar_contador_tablas_primer_nivel(t_memoria* memoria);
void aumentar_contador_tablas_segundo_nivel(t_memoria* memoria);
void agregar_tabla_de_primer_nivel_a_memoria(t_memoria* memoria, t_tabla_entradas_primer_nivel* tabla_primer_nivel);
void agregar_tabla_de_segundo_nivel_a_memoria(t_memoria* memoria, t_tabla_paginas_segundo_nivel* tabla_segundo_nivel);
void responder_pcb_a_cliente(t_pcb* pcb_actualizado , int cliente_fd, op_memoria_message MENSSAGE);
t_pcb* eliminar_proceso(t_pcb* pcb_proceso, t_memoria* memoria);
void eliminar_tablas_de_segundo_nivel(t_pcb* pcb_proceso, t_tabla_entradas_primer_nivel* tabla_primer_nivel, t_memoria* memoria);
void eliminar_paginas_de_memoria(t_tabla_paginas_segundo_nivel* tabla_segundo_nivel, t_memoria* memoria);
void eliminar_tabla_de_primer_nivel(t_tabla_entradas_primer_nivel* tabla_primer_nivel, t_memoria* memoria, int posicion_tabla_en_lista);
void eliminar_archivo_swap(t_log* logger, t_pcb* pcb_proceso);
void eliminar_tabla_de_la_lista_de_tablas_del_sistema(t_memoria* memoria, t_tabla_paginas_segundo_nivel* tabla_segundo_nivel);
//////////////////////////////SEMAFOROS////////////////////////////

pthread_mutex_t mutex_memoria_principal_bitmap;

#endif
