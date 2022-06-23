#include "memoria.h"


int main(void) {
	t_memoria* memoria = malloc(sizeof(t_memoria));

	memoria->memoria_log = log_create("memoria.log", "Modulo_Memoria", 1, LOG_LEVEL_DEBUG);
	log_info(memoria->memoria_log, "--------------------------------------------\n");

	t_config_memoria* config_memoria = create_config(memoria->memoria_log);

	memoria->memoria_config= config_memoria;

    int server_fd = start_memoria(memoria);

    memoria->server_fd = server_fd;

    memoria->tablas_primer_nivel = list_create();

    memoria->tablas_segundo_nivel = list_create();

    memoria->marcos_libres = list_create();

    memoria->id_tablas_primer_nivel = 0;
    memoria->id_tablas_segundo_nivel = 0;



    int tamanio_memoria = memoria->memoria_config->tamanio_memoria;
    memoria->espacio_memoria = malloc(tamanio_memoria);

    int tamanio_paginas = memoria->memoria_config->tamanio_pagina;
    log_info(memoria->memoria_log,"/// Se tienen %d marcos de %d bytes en memoria principal", tamanio_memoria / tamanio_paginas, tamanio_paginas);



    //Creo un hilo para lo q es manejar conexiones, el otro flujo puede seguir para pedirle cosas a la memoria desde consola
	pthread_t hilo_servidor;
	pthread_create(&hilo_servidor, NULL, manejar_conexion,(void*)memoria);
	pthread_join(hilo_servidor, NULL);

	//liberar_conexion(server_fd);
	//liberar_memoria();

	return 0;
}


//----------Tema de creacion de hilos-------------------
void manejar_conexion(void* void_args){
	t_memoria* memoria = (t_memoria*) void_args;

	pthread_mutex_t* semaforo_conexion = malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(semaforo_conexion, NULL);

	int server_fd = memoria->server_fd;
	    while(1){
	    	t_args_administrar_cliente* args_administrar_cliente = malloc(sizeof(t_args_administrar_cliente));
	    	args_administrar_cliente->semaforo_conexion = semaforo_conexion;
	        int cliente_fd = wait_client(server_fd, memoria->memoria_log, "Cliente", "Memoria");
	        args_administrar_cliente->socket = cliente_fd;
	        args_administrar_cliente->memoria = memoria;
	        pthread_t hilo_servidor;
	        pthread_create (&hilo_servidor, NULL, (void*)administrar_cliente,(void*) args_administrar_cliente);
	        pthread_detach(hilo_servidor);
	    }
	}


int administrar_cliente(t_args_administrar_cliente* args_administrar_cliente){
	int cliente_fd = args_administrar_cliente->socket;
	int op_code;
	t_memoria* memoria = args_administrar_cliente->memoria;
	while(1){

		recv(cliente_fd, &op_code, sizeof(int), MSG_WAITALL);
		pthread_mutex_lock(args_administrar_cliente->semaforo_conexion);
		op_memoria_message op_code_memoria = op_code;

		if(op_code_memoria == HANDSHAKE){

			hacer_handshake_con_cpu(cliente_fd, memoria);

		}else{
			t_pcb* pcb_cliente = deserializate_pcb_memoria(cliente_fd);

			if(op_code_memoria == NEW){

				iniciar_proceso(pcb_cliente, cliente_fd, memoria);

			}else if (op_code_memoria == DELETE){

				t_pcb* pcb_actualizado = eliminar_proceso(pcb_cliente, memoria);
				log_info(memoria->memoria_log, "SE ELIMINAN TODAS LAS ESTRUCTURAS DEL PROCESO %d EN MEMORIA", pcb_cliente->id);
				responder_pcb_a_cliente(pcb_actualizado, cliente_fd, OPERACION_EXITOSA);

			} else {
				log_warning(memoria->memoria_log, "Operacion desconocida\n");
			}
		}
		pthread_mutex_unlock(args_administrar_cliente->semaforo_conexion);
	}
	    return EXIT_SUCCESS;
}

void hacer_handshake_con_cpu(int cliente_fd, t_memoria* memoria){

	int tamanio_pagina = memoria->memoria_config->tamanio_pagina;
	int entradas_por_tabla = memoria->memoria_config->entradas_por_tabla;

	log_info(memoria->memoria_log, "CONECTADO A CPU, REALIZANDO HANDSHAKE.");
	void* a_enviar = malloc(2*sizeof(int));
	memcpy(a_enviar, &tamanio_pagina, sizeof(int));
	memcpy(a_enviar + sizeof(int), &entradas_por_tabla, sizeof(int));
	send_data_to_server(cliente_fd, a_enviar, 2*sizeof(int));
	log_info(memoria->memoria_log, "DATOS ENVIADOS.");
}


void iniciar_proceso(t_pcb* pcb_cliente, int cliente_fd, t_memoria* memoria){

	int id_proceso = pcb_cliente->id;
    int tamanio_proceso = pcb_cliente->processSize;


    log_info(memoria->memoria_log, "Iniciando proceso %d que pesa %d...", id_proceso, tamanio_proceso);

    t_pcb* pcb_actualizado = guardar_proceso_en_paginacion(pcb_cliente, memoria);


    if(pcb_actualizado->tabla_paginas != NULL){
    		responder_pcb_a_cliente(pcb_actualizado , cliente_fd, OPERACION_EXITOSA);
    		log_info(memoria->memoria_log, "----------> Se guarda el proceso [%d] en memoria\n", id_proceso);

    	}else{
    		responder_pcb_a_cliente(pcb_actualizado , cliente_fd, OPERACION_FALLIDA);
    		log_info(memoria->memoria_log, "----------> No hay lugar para guardar el proceso [%d] en memoria\n", id_proceso);

    	}
    //free(pcb_actualizado->tabla_paginas);
    free(pcb_actualizado);
}

void responder_pcb_a_cliente(t_pcb* pcb_actualizado , int cliente_fd, op_memoria_message MENSSAGE){
	if(pcb_actualizado != NULL){
		t_cpu_paquete* paquete = malloc(sizeof(t_cpu_paquete));
		void* pcb_serializate = serializate_pcb(pcb_actualizado, paquete, MENSSAGE);

		int code_operation = send_data_to_server(cliente_fd, pcb_actualizado, (paquete->buffer->size + sizeof(int) + sizeof(int)));
		if(code_operation < 0){
			error_show("OCURRIO UN PROBLEMA INTENTANDO RESPONDERLE AL CLIENTE, ERROR: IMPOSIBLE RESPONDER");
			exit(1);
		}
		free(pcb_serializate);
	}
}



t_pcb* guardar_proceso_en_paginacion(t_pcb* pcb_cliente, t_memoria* memoria){

	//_________________CREACION DE ARCHIVO DEL PROCESO_____________________

	char* path_archivo = memoria->memoria_config->path_swap;
	int id_proceso = pcb_cliente->id;

	char nombre[50];
	strcpy(nombre,  ".swap");
	char* id_proceso_char = string_itoa(id_proceso);

	strcat(id_proceso_char, nombre);
	strcat(path_archivo, id_proceso_char);

	FILE* archivo_proceso;

	archivo_proceso = fopen(path_archivo, "w" );
	log_info(memoria->memoria_log, "SE CREA EL ARCHIVO SWAP DEL PROCESO %d EN LA RUTA %s", id_proceso, path_archivo);

	//_____________________CREACION DE TABLAS______________________________
	int tamanio_proceso = pcb_cliente->processSize;
	int cant_marcos = memoria->memoria_config->marcos_proceso;

	int contador_marcos_por_escribir = cant_marcos;

	int paginas_necesarias = ceil((double) tamanio_proceso/ (double) memoria->memoria_config->tamanio_pagina);

	//obtengo la cantidad de tabla de paginas de segundo nivel necesarias ==> CANTIDAD DE ENTRADAS DE TABLA #1
	//ej: pag_necesarias = 16, marcos_por_proceso= 4 --> cant_tablas #2 = 4 --> cant_entradas #1 = 4
	int cant_tablas_segundo_necesarias = ceil(sqrt((double)paginas_necesarias));

	t_tabla_entradas_primer_nivel* tabla_primer_nivel = malloc(sizeof(t_tabla_entradas_primer_nivel*));

	aumentar_contador_tablas_primer_nivel(memoria);
	tabla_primer_nivel->id_proceso = pcb_cliente->id;
	tabla_primer_nivel->id_tabla = memoria->id_tablas_primer_nivel;

	t_list* entadas_tabla_nivel_uno = list_create();

	int paginas_guardadas = 0;

	for(paginas_guardadas = 0; paginas_guardadas < cant_tablas_segundo_necesarias; paginas_guardadas++){

		t_tabla_paginas_segundo_nivel* tabla_segundo_nivel = malloc(sizeof(t_tabla_paginas_segundo_nivel*));

		aumentar_contador_tablas_segundo_nivel(memoria);
		tabla_segundo_nivel->id_tabla = memoria->id_tablas_segundo_nivel;

		t_list* paginas_tabla_segundo_nivel = list_create();

		//Ver si paginas guardadas aumenta desp de entrar en el for, sino habria q agregar un +1
		if((paginas_guardadas + 1) == cant_tablas_segundo_necesarias){ //Creo que este if esta mal, es para la ultima tabla o primer tabla para aquellos q solo tengan una entrada
			int i= 0;
			for(i=0; i < paginas_necesarias; i++){
				int numero_pagina = 0;
				t_pagina_segundo_nivel* pagina_segundo_nivel = malloc(sizeof(t_pagina_segundo_nivel));
				pagina_segundo_nivel->id_pagina = numero_pagina + 1;

           	if(contador_marcos_por_escribir > 0){
           		pagina_segundo_nivel->presencia = 1;
           		//Asignar marco
           		//pagina_segundo_nivel->frame_principal ??
              	contador_marcos_por_escribir = contador_marcos_por_escribir - 1;
          			}else{
           			pagina_segundo_nivel->presencia = 0;
           			//pagina_segundo_nivel->frame_principal = NULL
           	}

				pagina_segundo_nivel->uso = 1;
				pagina_segundo_nivel->modificado= 1; //??

				list_add(paginas_tabla_segundo_nivel, pagina_segundo_nivel);
				//tabla_nivel_dos = agregar pagina

			}
			tabla_segundo_nivel->paginas_segundo_nivel = paginas_tabla_segundo_nivel;
		}else{
			int i= 0;
			for(i=0; i < cant_tablas_segundo_necesarias; i++){
				int numero_pagina = 0;
				t_pagina_segundo_nivel* pagina_segundo_nivel = malloc(sizeof(t_pagina_segundo_nivel));
				pagina_segundo_nivel->id_pagina = numero_pagina + 1;

        		if(contador_marcos_por_escribir > 0){
           		pagina_segundo_nivel->presencia = 1;
           		//Asignar marco
           		//pagina_segundo_nivel->frame_principal ??
              	contador_marcos_por_escribir = contador_marcos_por_escribir - 1;
          			}else{
           			pagina_segundo_nivel->presencia = 0;
           			//pagina_segundo_nivel->frame_principal = NULL
           	}

				pagina_segundo_nivel->uso = 1;
				pagina_segundo_nivel->modificado=1; //??

				list_add(paginas_tabla_segundo_nivel, pagina_segundo_nivel);
				paginas_necesarias = paginas_necesarias - 1;

			}

			tabla_segundo_nivel->paginas_segundo_nivel = paginas_tabla_segundo_nivel;

		}
		list_add(entadas_tabla_nivel_uno, tabla_segundo_nivel);
		agregar_tabla_de_segundo_nivel_a_memoria(memoria, tabla_segundo_nivel);
}
	tabla_primer_nivel->entradas = entadas_tabla_nivel_uno;

	agregar_tabla_de_primer_nivel_a_memoria(memoria, tabla_primer_nivel);

	pcb_cliente->tabla_paginas = tabla_primer_nivel->id_tabla;


	//_________________CERRADO DE ARCHIVO DEL PROCESO_____________________

	fclose(archivo_proceso);
	log_info(memoria->memoria_log, "SE CIERRA EL ARCHIVO SWAP DEL PROCESO %d DE LA RUTA %s", id_proceso, path_archivo);

	return pcb_cliente;

}

void agregar_tabla_de_primer_nivel_a_memoria(t_memoria* memoria, t_tabla_entradas_primer_nivel* tabla_primer_nivel){
	t_list* tablas_actuales = memoria->tablas_primer_nivel;
	list_add(tablas_actuales, tabla_primer_nivel);
}

void agregar_tabla_de_segundo_nivel_a_memoria(t_memoria* memoria, t_tabla_paginas_segundo_nivel* tabla_segundo_nivel){
	t_list* tablas_actuales = memoria->tablas_segundo_nivel;
	list_add(tablas_actuales, tabla_segundo_nivel);
}

void aumentar_contador_tablas_primer_nivel(t_memoria* memoria){
	int contador_actual = memoria->id_tablas_primer_nivel;
	int contador_actualizado = contador_actual + 1;

	memoria->id_tablas_primer_nivel = contador_actualizado;
}

void aumentar_contador_tablas_segundo_nivel(t_memoria* memoria){
	int contador_actual = memoria->id_tablas_segundo_nivel;
	int contador_actualizado = contador_actual + 1;

	memoria->id_tablas_segundo_nivel = contador_actualizado;
}

//-------------------------------ELIMINAR PROCESO----------------------------------------------------
t_pcb* eliminar_proceso(t_pcb* pcb_proceso, t_memoria* memoria){

log_info(memoria->memoria_log, "INICIO A BORRAR LAS ESTRUCTURAS DEL PROCESO %d EN MEMORIA", pcb_proceso->id);

int* id_tabla_primer_nivel = malloc(sizeof(int));

id_tabla_primer_nivel = pcb_proceso->tabla_paginas;

t_list* tablas_actuales = memoria->tablas_primer_nivel;

int tamanio_lista_primer_nivel = list_size(tablas_actuales);

int recorrido_tabla;

	for(recorrido_tabla = 0 ; recorrido_tabla < tamanio_lista_primer_nivel ; recorrido_tabla++){

			if(list_get(tablas_actuales, recorrido_tabla)!= NULL){
				t_tabla_entradas_primer_nivel* tabla_primer_nivel = list_get(tablas_actuales, recorrido_tabla);
				int id_tabla = tabla_primer_nivel->id_tabla;

					if(id_tabla == (int)id_tabla_primer_nivel){

						eliminar_tablas_de_segundo_nivel(pcb_proceso, tabla_primer_nivel, memoria);
						log_info(memoria->memoria_log, "TABLAS DE SEGUNDO NIVEL DEL PROCESO %d ELIMINADAS TOTALMENTE", pcb_proceso->id);

						eliminar_tabla_de_primer_nivel(tabla_primer_nivel, memoria, id_tabla);

						eliminar_archivo_swap(memoria->memoria_log, pcb_proceso);
						//list_destroy(tabla_primer_nivel->entradas);
						//free(tabla_primer_nivel);
						pcb_proceso->tabla_paginas = NULL;
						log_info(memoria->memoria_log, "TABLA DE PRIMER NIVEL DEL PROCESO %d ELIMINADA", pcb_proceso->id);

					}
			}

	}

	return pcb_proceso;

}

void eliminar_tablas_de_segundo_nivel(t_pcb* pcb_proceso, t_tabla_entradas_primer_nivel* tabla_primer_nivel, t_memoria* memoria){

	t_list* entradas_de_tabla = tabla_primer_nivel->entradas;

	int recorrido_entradas = 0;

	int tamanio_lista_segundo_nivel = list_size(entradas_de_tabla);

	for(recorrido_entradas = 0 ; recorrido_entradas < tamanio_lista_segundo_nivel ; recorrido_entradas++){ //elimina las tablas de nivel dos de a una

		t_tabla_paginas_segundo_nivel* tabla_segundo_nivel = list_get(entradas_de_tabla, recorrido_entradas);
		int id_tabla_segundo_nivel = tabla_segundo_nivel->id_tabla;

		eliminar_paginas_de_memoria(tabla_segundo_nivel, memoria);

		eliminar_tabla_de_la_lista_de_tablas_del_sistema(memoria, tabla_segundo_nivel);

		log_info(memoria->memoria_log, "TABLA DE SEGUNDO NIVEL %d DEL PROCESO %d ELIMINADA", id_tabla_segundo_nivel, pcb_proceso->id);
	}

}

void eliminar_tabla_de_la_lista_de_tablas_del_sistema(t_memoria* memoria, t_tabla_paginas_segundo_nivel* tabla_segundo_nivel){

	t_list* tablas_actuales_de_segundo_nivel = memoria->tablas_segundo_nivel;

	int tamanio_lista_segundo_nivel = list_size(tablas_actuales_de_segundo_nivel);

	int tabla_a_eliminar = tabla_segundo_nivel->id_tabla;

	int recorrido_lista;

		for(recorrido_lista = 0 ; recorrido_lista < tamanio_lista_segundo_nivel ; recorrido_lista++){

				if(list_get(tablas_actuales_de_segundo_nivel, recorrido_lista)!= NULL){
					t_tabla_paginas_segundo_nivel* tabla_segundo_nivel_de_lista = list_get(tablas_actuales_de_segundo_nivel, recorrido_lista);
					int id_tabla = tabla_segundo_nivel_de_lista->id_tabla;

						if(tabla_a_eliminar == id_tabla){

							list_remove_and_destroy_element(tablas_actuales_de_segundo_nivel, recorrido_lista, tabla_segundo_nivel); //ver como funciona esto
						}
				}

		}

}

void eliminar_paginas_de_memoria(t_tabla_paginas_segundo_nivel* tabla_segundo_nivel, t_memoria* memoria){

//Si las tablas contienen paginas con bit de presencia en 1 ==> liberar espacio de memoria es decir, poner espacio de memoria en 0

}

void eliminar_tabla_de_primer_nivel(t_tabla_entradas_primer_nivel* tabla_primer_nivel, t_memoria* memoria, int posicion_tabla_en_lista){

// Eliminar tabla de primer nivel de lista de tablas de primer nivel

	t_list* lista_de_tablas_primer_nivel = memoria->tablas_primer_nivel;
	list_remove_and_destroy_element(lista_de_tablas_primer_nivel, posicion_tabla_en_lista, tabla_primer_nivel); //ver como funciona esto

//obtener id archivo del proceso



}

void eliminar_archivo_swap(t_log* logger, t_pcb* pcb_proceso){
	// Eliminar archivo del proceso
	log_info(logger, "ARCHIVO SWAP DEL PROCESO %d ELIMINADO", pcb_proceso->id);
}

