#ifndef UTILS_H_
#define UTILS_H_

#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
#include<sys/socket.h>
#include<commons/bitarray.h>
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
#include<conexion_servidor.h>
#include<estructuras.h>
#include <pthread.h>

//Funciones q podrian estar en shared y son utiles para el modulo


t_bitarray* frames_ocupados_principal;
int cant_frames_principal;
int cant_frames_virtual;


#endif

