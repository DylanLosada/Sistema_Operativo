#include "funciones_memoria.h"
#include "memoria.h"

//Funciones q podrian estar en shared y son utiles para el modulo

t_log* logger;

// Lee la configuración del archivo de configuracion
t_config* leer_config(char* path_config){
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

int esperar_cliente(int socket_servidor){
    struct sockaddr_in dir_cliente;
    int tam_direccion = sizeof(struct sockaddr_in);

    int socket_cliente = accept(socket_servidor, (void*) &dir_cliente, &tam_direccion);

    log_info(logger, "Se conecto un cliente!");

    return socket_cliente;
}

int recibir_operacion(int socket_cliente){
    int cod_op;
    if(recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL) != 0)
        return cod_op;
    else
    {
        close(socket_cliente);
        return -1;
    }
}

// Servidor:

int iniciar_servidor(char* ip, char* puerto){
    int socket_servidor;

    struct addrinfo hints, *servinfo, *p;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(ip, puerto, &hints, &servinfo);

    for (p=servinfo; p != NULL; p = p->ai_next){
        if ((socket_servidor = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
            continue;

        if (bind(socket_servidor, p->ai_addr, p->ai_addrlen) == -1) {
            close(socket_servidor);
            continue;
        }
        break;
    }

    listen(socket_servidor, SOMAXCONN);

    freeaddrinfo(servinfo);

    log_trace(logger, "Listo para escuchar a mi cliente");

    return socket_servidor;
}

int puedo_guardar_n_paginas(int paginas){
    int cantidadFramesLibresMP = frames_disponibles_en(MEMORIA_PRINCIPAL);
    int cantidadFramesLibresMV = frames_disponibles_en(MEMORIA_VIRTUAL);

    if((cantidadFramesLibresMP + cantidadFramesLibresMV) >= paginas){
        return 1;
    }else{
        return 0;
    }
}

void liberar_memoria(){
    liberar_memoria_paginada();
    free(memoria_principal);
    log_destroy(logger);
}


//void liberar_memoria_paginada(){
//	bitarray_destroy(frames_ocupados_principal);
//	bitarray_destroy(frames_ocupados_virtual);
//	free(datos_memoria);
//	free(datos_memoria_virtual);
//	pthread_mutex_lock(&mutex_tablas_de_paginas);
//	list_destroy_and_destroy_elements(tablas_de_paginas, (void*)eliminar_tabla_de_paginas);
//	pthread_mutex_unlock(&mutex_tablas_de_paginas);
//}

//---------------------------- Paginacion------------------------------

int iniciar_memoria_paginada(){
    memoria_principal = malloc(config_memoria.tamanio_memoria);

    if(memoria_principal == NULL){
        //NO SE RESERVO LA MEMORIA
        perror("MALLOC FAIL!\n");
        return 0;
    }

    int tamanio_paginas = config_memoria.tamanio_pagina;
    //char * path_memoria_virtual = config_memoria.path_swap; ver tema de memoria virtual, porq hay tlb y el swap no es con memoria virtual sino con archivos

    cant_frames_principal = config_memoria.tamanio_memoria / tamanio_paginas;

    log_info(logger,"/// Se tienen %d marcos de %d bytes en memoria principal",cant_frames_principal, config_memoria.tamanio_pagina);
    int bytesAsignar = bit_to_byte(cant_frames_principal);
    char* datos_memoria = asignar_bytes_de_memoria(bytesAsignar);

    if(datos_memoria == NULL){
        perror("MALLOC FAIL!\n");
        return 0;
    }
    //pone en 0 los primeros (cant_frames_principal/8) de datos de memoria
    memset(datos_memoria,0,cant_frames_principal/8);
    frames_ocupados_principal = bitarray_create_with_mode(datos_memoria, cant_frames_principal/8, MSB_FIRST);

    return 1;
}

int bit_to_byte(int bits){
    int bytes;
    if(bits < 8)
        bytes = 1;
    else
    {
        double c = (double) bits;
        bytes = ceil(c/8.0);
    }

    return bytes;
}

char* asignar_bytes_de_memoria(int bytes){
    char* arrayBytes;
    arrayBytes = malloc(bytes);
    memset(arrayBytes,0,bytes);
    return arrayBytes;
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
