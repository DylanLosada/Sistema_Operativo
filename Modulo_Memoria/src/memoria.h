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

#define MEMORIA_PRINCIPAL 0
#define MEMORIA_VIRTUAL 1

//Funciones que sirven para memoria tales como estructuras o funciones

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
	int puerto;
	int tamanio_memoria;
	int tamanio_pagina;
	int entradas_por_tabla;
	int retardo_memoria;
	char * algoritmo_reemplazo;
	int marcos_proceso;
	int retardo_swap;
	char * path;
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

bool funcionando;

int cant_frames_principal;
int cant_frames_virtual;


int puedo_guardar_n_paginas(int paginas){
    int cantidadFramesLibresMP = frames_disponibles_en(MEMORIA_PRINCIPAL);
    int cantidadFramesLibresMV = frames_disponibles_en(MEMORIA_VIRTUAL);

    if((cantidadFramesLibresMP + cantidadFramesLibresMV) >= paginas){
        return 1;
    }else{
        return 0;
    }
}

//Ver funcion robada
int frames_disponibles_en(int memoria){
    int espaciosLibres = 0;
    int desplazamiento = 0;

    if(memoria == MEMORIA_PRINCIPAL){
        while(desplazamiento < cant_frames_principal){
            pthread_mutex_lock(&mutex_memoria_principal_bitmap);
            if(bitarray_test_bit(frames_ocupados_principal,desplazamiento) == 0){
                espaciosLibres++;
            }
            pthread_mutex_unlock(&mutex_memoria_principal_bitmap);
            desplazamiento++;
        }
    }else if(memoria == MEMORIA_VIRTUAL){
        while(desplazamiento < cant_frames_virtual){

            pthread_mutex_lock(&mutex_memoria_virtual_bitmap);
            if(bitarray_test_bit(frames_ocupados_virtual, desplazamiento) == 0){
                espaciosLibres++;
            }
            pthread_mutex_unlock(&mutex_memoria_virtual_bitmap);
            desplazamiento++;
        }
    }
    return espaciosLibres;
}

//////////////////////////////SEMAFOROS////////////////////////////

pthread_mutex_t mutex_memoria_principal_bitmap;


#endif
