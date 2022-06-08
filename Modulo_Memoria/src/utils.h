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
#include<unistd.h>
#include<netdb.h>
#include<commons/collections/list.h>
#include<commons/collections/queue.h>
#include<commons/config.h>
#include<dirent.h>
#include<errno.h>

//Funciones q podrian estar en shared y son utiles para el modulo

t_log* logger;

// Lee la configuración del archivo de configuracion
t_config_memoria* leer_config(char* path_config){
    return config_create(path_config);
}
int existe_dir(char* path)
{
	DIR* dir = opendir(path);
	if (dir) {
		closedir(dir);
		return 1;
	} else if (ENOENT == errno) {
		return 0;
	} else {
		return -1;
	}
}

void* recibir_buffer(int* size, int socket_cliente){
    void * buffer;
    recv(socket_cliente, size, sizeof(int), MSG_WAITALL); /* MSG_WAITALL es un flag Wait for a full request.  */
    buffer = malloc(*size);
    recv(socket_cliente, buffer, *size, MSG_WAITALL);

    return buffer;
}

int leer_entero(void * buffer, int desplazamiento){
    int leido;
    memcpy(&leido, buffer + desplazamiento * sizeof(int) , sizeof(int));
    return leido;
}

char* leer_string(void* buffer, int desplazamiento){

    int tamanio = leer_entero(buffer, desplazamiento);

    char* valor = malloc(tamanio);
    memcpy(valor, buffer + (desplazamiento+1)*sizeof(int), tamanio);

    return valor;
}


#endif

