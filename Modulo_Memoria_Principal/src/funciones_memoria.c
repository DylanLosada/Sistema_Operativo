#include "funciones_memoria.h"

t_config_memoria* create_config(t_log* log){
	t_config* config = config_create("/home/utnso/tp-2022-1c-SanguchitOS/Modulo_Memoria_Principal/memoria.config");
	t_config_memoria* memoria_config = malloc(sizeof(t_config_memoria));

	if(config == NULL){
		log_error(log, "No se ha encontrado memoria.config");
		return NULL;
	}
	memoria_config->puerto=config_get_string_value(config,"PUERTO_ESCUCHA");
	memoria_config->tamanio_memoria = config_get_int_value(config, "TAM_MEMORIA");
	memoria_config->tamanio_pagina = config_get_int_value(config, "TAM_PAGINA");
	memoria_config->path_swap = config_get_string_value(config, "PATH_SWAP");
	memoria_config->entradas_por_tabla = config_get_int_value(config, "ENTRADAS_POR_TABLA");
	memoria_config->retardo_memoria = config_get_int_value(config, "RETARDO_MEMORIA");
	memoria_config->algoritmo_reemplazo = strcmp(config_get_string_value(config, "ALGORITMO_REEMPLAZO"), "CLOCK") == 0 ? CLOCK : CLOCK_MODIFICADO;
	memoria_config->marcos_proceso = config_get_int_value(config, "MARCOS_POR_PROCESO");
	memoria_config->retardo_swap = config_get_int_value(config, "RETARDO_SWAP");
	memoria_config->path_swap = config_get_string_value(config, "PATH_SWAP");

	log_info(log, "Archivo de configuracion cargado con exito");

	return memoria_config;

}

// INICIA SERVER ESCUCHANDO EN IP:PUERTO
int start_memoria(t_memoria* memoria){
	return create_server_connection(memoria->memoria_config->puerto, memoria->memoria_log, "MEMORIA LISTA PARA RECIBIR INSTRUCCIONES");
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


void liberar_memoria(t_memoria* memoria){
    log_destroy(memoria->memoria_log);
    free(memoria);
}

int obtener_marco_de_memoria(t_memoria* memoria){
	t_list* marcos_libres = memoria->marcos_libres;

	return list_remove(marcos_libres, 0);
}

void agregar_marco_de_memoria_a_lista(t_memoria* memoria, int marco){
	t_list* marcos_libres = memoria->marcos_libres;
	list_add(marcos_libres, marco);
}

//void liberar_memoria_paginada(){
//	bitarray_destroy(frames_ocupados_principal);
//	free(datos_memoria);
//	pthread_mutex_lock(&mutex_tablas_de_paginas);
//	list_destroy_and_destroy_elements(tablas_de_paginas, (void*)eliminar_tabla_de_paginas);
//	pthread_mutex_unlock(&mutex_tablas_de_paginas);
//}
