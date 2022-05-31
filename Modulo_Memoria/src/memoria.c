#include "memoria.h"
#include "utils.h"


int main(void) {
	int socketCliente;

	char* path_config_memoria = "/home/utnso/tp-2022-1c-SanguchitOS-main/Modulo_Memoria/memoria.config";
    funcionando=true;
    t_config* config = leer_config(path_config_memoria);
    config_memoria.tamanio_memoria = config_get_int_value(config, "TAM_MEMORIA");
	config_memoria.tamanio_pagina = config_get_int_value(config, "TAM_PAGINA");
	config_memoria.entradas_por_tabla = config_get_int_value(config, "ENTRADAS_POR_TABLA");
	config_memoria.retardo_memoria = config_get_int_value(config, "RETARDO_MEMORIA");
	config_memoria.algoritmo_reemplazo = config_get_string_value(config, "ALGORITMO_REEMPLAZO");
	config_memoria.marcos_proceso = config_get_int_value(config, "MARCOS_POR_PROCESO");
	config_memoria.retardo_swap = config_get_int_value(config, "RETARDO_SWAP");
	config_memoria.path = config_get_string_value(config, "PATH_SWAP");

	// Ver funciones y temas sobre paginacion jerarquizada
	// en clase se vio que hay una tabla por procesos
	//la tabla de marcos seria la memoria ram, la que es para todos igual

	int tamanio_paginas = config_memoria.tamanio_pagina;

	t_log* logger = iniciar_logger();
	logger = log_create("../memoria.log", "Modulo_Memoria", 0, LOG_LEVEL_DEBUG);
	log_info(logger, "--------------------------------------------\n");

	memoria_principal = malloc(config_memoria.tamanio_memoria);
	log_info(logger,"Direccion inicial de memoria: %d",memoria_principal);


	//La IP de la memoria esta en CPU config, habria q ver como hacer q memoria lo sepa,
	//tal vez hacer una carpeta compartida para todos, como una shared
	//config_memoria.ip = config_get_int_value(config, "IP");
	int server_fd = iniciar_servidor(config_memoria.ip, config_memoria.puerto);

	//Aca lo que hago es mientras funcionando este en true, se espera al cliente. Hay que ver
	//El tema de semaforos, tal vez esto podria verse ,mejor o utilizar hilos como hago

	while (funcionando) {
		socketCliente = esperar_cliente(server_fd, 5);
		if (socketCliente == -1)
			continue;

			pthread_t hiloProceso;
			pthread_create(&hiloProceso,NULL,(void*)administrar_Proceso,socketCliente);
			pthread_detach(hiloProceso);
	}


	liberar_conexion(server_fd);

	return 0;
	}

void administrar_cliente(int socketCliente){

}


//Administrar proceso tiene que hacer toda la logica, acordate que este socket puede llegar desde kernell o desde cpu
//kernell lo unico que hace es enviar pcbs de procesos nuevos para almacenarlos en memoria
//cpu manda el pcb con cierta instruccion. Dependiendo la instruccion es lo que hay que hacer, en estos casos cpu deberia quedarse esperando a q memoria le mande el ok

