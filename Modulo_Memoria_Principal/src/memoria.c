#include "memoria.h"


/*
 *   TODO
 * * VER TEMA DE CLOCK Y CLOCK MODIFICADO
 * * SWAP Y RE-SWAP
 * * HACER CONSULTAS DE SI EL GRADO DE MULTIPROGRAMACION VIENE ACORDE A LA CANTIDAD DE FRAMES DE MEMORIA
 * *ALGORITMO DE REEMPLAZO ES DEL PROCESO O DEL SISTEMA EN GENERAL?
 *
 * CANT FRAMES 20
 * GRADO MULTI 5
 * FRAMES POR PROCESO 4
 *
 * -------> 20 FRAMES
 *
 *
 * * INSTRUCCIONES READ/COPY/WRITE
 * * EL TP ACLARA QUE EL OPCODE DELETE NO ELIMINA TABLA DE PAGINAS SINO QUE SOLO BORRA SU CONTENIDO Y ARCHIVO SWAP
 * * UN PROCESO PUEDE ACCEDER A UN ESPACIO DE MEMORIA DE OTRRO PROCESO? ES POR EL TEMA DE LAS INSTRUCCIONES Y LAS DIRECIIONES QUE LLEGUEN COMO PARAMETRO
 * * LA LISTA DE ENTRADAS DDE TABLA DE PRIMER NIVEL NO ES UNA LISTA DE TABLAS SINO UNA LISTA DE ID CORREGIR!!
 */

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

    int cantidad_de_frames= (memoria->memoria_config->tamanio_memoria)/(memoria->memoria_config->tamanio_pagina);

    inicializar_lista_de_marcos_libres(cantidad_de_frames, memoria);

    memoria->id_tablas_primer_nivel = 0;
    memoria->id_tablas_segundo_nivel = 0;

    int tamanio_memoria = memoria->memoria_config->tamanio_memoria;
    memoria->espacio_memoria = malloc(tamanio_memoria);

    int tamanio_paginas = memoria->memoria_config->tamanio_pagina;
    log_info(memoria->memoria_log,"/// Se tienen %d marcos de %d bytes en memoria principal", tamanio_memoria / tamanio_paginas, tamanio_paginas);

    //-------------------CREO DIRECTORIO PARA LOS ARCHIVOS SWAP------------------------------

    	char* ruta_directorio = "/home/utnso/tp-2022-1c-SanguchitOS/";

    	char* swap = "swap";

    	//strcat(ruta_directorio, swap);

    	int directorio = mkdir("/home/utnso/tp-2022-1c-SanguchitOS/swap", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);


    //Creo un hilo para lo q es manejar conexiones, el otro flujo puede seguir para pedirle cosas a la memoria desde consola
	pthread_t hilo_servidor;
	pthread_create(&hilo_servidor, NULL, manejar_conexion,(void*)memoria);
	pthread_join(hilo_servidor, NULL);

	//liberar_conexion(server_fd);
	//liberar_memoria();

	return 0;
}

void inicializar_lista_de_marcos_libres(int cantidad_de_frames, t_memoria* memoria){

	int frame_actual;
	t_list* marcos_libres_para_asignar = list_create();

	for(frame_actual = 0; frame_actual < cantidad_de_frames; frame_actual++){

		//memoria->marcos_libres[frame_actual]= frame_actual+1;
		list_add(marcos_libres_para_asignar, frame_actual);
		log_info(memoria->memoria_log,"Se tienen el valor %d en la posicion de la lista %d", list_get(marcos_libres_para_asignar, frame_actual), marcos_libres_para_asignar->elements_count);
	}
	memoria->marcos_libres = marcos_libres_para_asignar;
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

		}
		// CPU Y SUS PEDIDOS
		else if (op_code_memoria == READ){
			// ESTE ESTA BIEN
			int dir_fisica;
			recv(cliente_fd, &dir_fisica, sizeof(int), MSG_WAITALL);
			log_info(memoria->memoria_log, "LA DIRECCION DE MEMORIA LEIDA: %d", dir_fisica);
			// data es el valor leido.
			send(cliente_fd, &data, sizeof(int), 0);
		}
		else if (op_code_memoria == COPY){
			int size;
			int primera_direccion;
			int segunda_direccion;
			recv(socket, &size, sizeof(int), MSG_WAITALL);
			void* stream = malloc(size);
			recv(socket, stream, size, MSG_WAITALL);
			memcpy(&primera_direccion, stream, sizeof(int));
			memcpy(&segunda_direccion, stream + sizeof(int), sizeof(int));



			log_info(memoria->memoria_log, "SE EJECUTO EL COPIADO DE DATOS DE %d A %d", primera_direccion, segunda_direccion);
			send(cliente_fd, &OK, sizeof(int), 0);
		}
		else if (op_code_memoria == WRITE){
			int size;
			int direccion;
			int valor;
			recv(socket, &size, sizeof(int), MSG_WAITALL);
			void* stream = malloc(size);
			recv(socket, stream, size, MSG_WAITALL);
			memcpy(&direccion, stream, sizeof(int));
			memcpy(&valor, stream + sizeof(int), sizeof(int));



			log_info(memoria->memoria_log, "SE EJECUTO LA ESCRITURA EN LA DIR. %d, VALOR: %d", direccion, valor);
			send(cliente_fd, &OK, sizeof(int), 0);
		}
		else if (op_code_memoria == TABLA_SEGUNDO_NIVEL){
			// ESTA BIEN ESTO
			t_administrar_mmu* administrar_mmu = malloc(sizeof(t_administrar_mmu));
			deserialize_mmu_memoria(administrar_mmu, cliente_fd);
			log_info(memoria->memoria_log, "LA TABLA DE PRIMER NIVEL: %d Y LA ENTRADA DE PRIMER NIVEL %d", administrar_mmu->tabla_nivel, administrar_mmu->entrada_nivel);



			send(cliente_fd, &tabla_segundo_nivel, sizeof(int), 0);
		}
		else if (op_code_memoria == MARCO){
			// ESTA BIEN ESTO
			t_administrar_mmu* administrar_mmu = malloc(sizeof(t_administrar_mmu));
			deserialize_mmu_memoria(administrar_mmu, cliente_fd);
			log_info(memoria->memoria_log, "LA TABLA DE SEGUNDO NIVEL: %d Y LA ENTRADA DE SEGUNDO NIVEL %d", administrar_mmu->tabla_nivel, administrar_mmu->entrada_nivel);


			send(cliente_fd, &marco, sizeof(int), 0);
		}
		// KERNEL
		else{
			t_pcb* pcb_cliente = deserializate_pcb_memoria(cliente_fd);

			if(op_code_memoria == NEW){

				iniciar_proceso(pcb_cliente, cliente_fd, memoria);

			}else if (op_code_memoria == DELETE){

				t_pcb* pcb_actualizado = eliminar_proceso(pcb_cliente, memoria);
				log_info(memoria->memoria_log, "SE ELIMINAN TODAS LAS ESTRUCTURAS DEL PROCESO %d EN MEMORIA", pcb_cliente->id);
				responder_pcb_a_cliente(pcb_actualizado, cliente_fd, OPERACION_EXITOSA);

			} else if(op_code_memoria == SWAP){

				hacer_swap_del_proceso(pcb_cliente, memoria);
				log_info(memoria->memoria_log, "SE SE HACE SWAP DEL PROCESO %d PASANDO LAS PAGINAS DE MEMORIA A SU ARCHIVO", pcb_cliente->id);
				responder_pcb_a_cliente(pcb_cliente, cliente_fd, OPERACION_EXITOSA);

			}
			else{
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
    		responder_pcb_a_cliente(pcb_actualizado , cliente_fd, NEW);
    		log_info(memoria->memoria_log, "----------> Se guarda el proceso [%d] en memoria\n", id_proceso);

    	}else{
    		responder_pcb_a_cliente(pcb_actualizado , cliente_fd, ERROR);
    		log_info(memoria->memoria_log, "----------> No hay lugar para guardar el proceso [%d] en memoria\n", id_proceso);

    	}
    //free(pcb_actualizado->tabla_paginas);
    free(pcb_actualizado);
}

void responder_pcb_a_cliente(t_pcb* pcb_actualizado , int cliente_fd, op_memoria_message MENSSAGE){
	if(pcb_actualizado != NULL){
		t_cpu_paquete* paquete = malloc(sizeof(t_cpu_paquete));
		void* pcb_serializate = serializate_pcb(pcb_actualizado, paquete, MENSSAGE);

		int code_operation = send(cliente_fd, pcb_serializate, (paquete->buffer->size + sizeof(int) + sizeof(int)), 0);
		if(code_operation < 0){
			error_show("OCURRIO UN PROBLEMA INTENTANDO RESPONDERLE AL CLIENTE, ERROR: IMPOSIBLE RESPONDER");
			exit(1);
		}
		free(pcb_serializate);
	}
}



t_pcb* guardar_proceso_en_paginacion(t_pcb* pcb_cliente, t_memoria* memoria){

	//_________________CREACION DE ARCHIVO DEL PROCESO_____________________

	char* path_archivo = obtener_path_swap_del_archivo_del_proceso(pcb_cliente, memoria);

	FILE* archivo_proceso;

	archivo_proceso = fopen(path_archivo, "w" );
	log_info(memoria->memoria_log, "SE CREA EL ARCHIVO SWAP DEL PROCESO %d EN LA RUTA %s", pcb_cliente->id, path_archivo);

	//_____________________CREACION DE TABLAS______________________________
	int tamanio_proceso = pcb_cliente->processSize;

	int contador_marcos_disponibles_por_proceso = memoria->memoria_config->marcos_proceso;

	int paginas_necesarias = ceil((double) tamanio_proceso/ (double) memoria->memoria_config->tamanio_pagina);

	//obtengo la cantidad de tabla de paginas de segundo nivel necesarias ==> CANTIDAD DE ENTRADAS DE TABLA #1
	//ej: pag_necesarias = 16, marcos_por_proceso= 4 --> cant_tablas #2 = 4 --> cant_entradas #1 = 4
	int cant_tablas_segundo_necesarias = ceil(sqrt((double) paginas_necesarias));

	t_tabla_entradas_primer_nivel* tabla_primer_nivel = malloc(sizeof(t_tabla_entradas_primer_nivel*));

	aumentar_contador_tablas_primer_nivel(memoria);
	tabla_primer_nivel->id_proceso = pcb_cliente->id;
	tabla_primer_nivel->id_tabla = memoria->id_tablas_primer_nivel;

	t_list* entadas_tabla_nivel_uno = list_create();

	int tablas_guardadas;

	for(tablas_guardadas = 0; tablas_guardadas < cant_tablas_segundo_necesarias; tablas_guardadas++){

		t_tabla_paginas_segundo_nivel* tabla_segundo_nivel = malloc(sizeof(t_tabla_paginas_segundo_nivel*));

		aumentar_contador_tablas_segundo_nivel(memoria);
		tabla_segundo_nivel->id_tabla = memoria->id_tablas_segundo_nivel;

		t_list* paginas_tabla_segundo_nivel = list_create();

		if((tablas_guardadas + 1) == cant_tablas_segundo_necesarias){
			int i= 0;
			for(i=0; i < paginas_necesarias; i++){
				int numero_pagina_de_tabla = 0;
				t_pagina_segundo_nivel* pagina_segundo_nivel = malloc(sizeof(t_pagina_segundo_nivel));
				pagina_segundo_nivel->id_pagina = numero_pagina_de_tabla + 1;

           	if(contador_marcos_disponibles_por_proceso > 0){
           		pagina_segundo_nivel->presencia = 1;
           		int marco_memoria_principal = obtener_marco_de_memoria(memoria);
           		pagina_segundo_nivel->frame_principal = marco_memoria_principal;
              	contador_marcos_disponibles_por_proceso = contador_marcos_disponibles_por_proceso - 1;
          			}else{
           			pagina_segundo_nivel->presencia = 0;
           			pagina_segundo_nivel->frame_principal = -1;
           	}

				pagina_segundo_nivel->uso = 1;
				pagina_segundo_nivel->modificado= 1; //??

				list_add(paginas_tabla_segundo_nivel, pagina_segundo_nivel);
				//tabla_nivel_dos = agregar pagina

			}
			tabla_segundo_nivel->paginas_segundo_nivel = paginas_tabla_segundo_nivel;
		}else{
			int pagina;
			for(pagina = 0; pagina < cant_tablas_segundo_necesarias; pagina++){
				int numero_pagina_de_tabla = 0;
				t_pagina_segundo_nivel* pagina_segundo_nivel = malloc(sizeof(t_pagina_segundo_nivel));
				pagina_segundo_nivel->id_pagina = numero_pagina_de_tabla + 1;

        		if(contador_marcos_disponibles_por_proceso > 0){
           		pagina_segundo_nivel->presencia = 1;
           		int marco_memoria_principal = obtener_marco_de_memoria(memoria);
				pagina_segundo_nivel->frame_principal = marco_memoria_principal;
              	contador_marcos_disponibles_por_proceso = contador_marcos_disponibles_por_proceso - 1;
          			}else{
           			pagina_segundo_nivel->presencia = 0;
           			pagina_segundo_nivel->frame_principal = -1;
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

	int id_tabla = tabla_primer_nivel->id_tabla;

	pcb_cliente->tabla_paginas = id_tabla;


	//_________________CERRADO DE ARCHIVO DEL PROCESO_____________________

	fclose(archivo_proceso);
	log_info(memoria->memoria_log, "SE CIERRA EL ARCHIVO SWAP DEL PROCESO %d DE LA RUTA %s", pcb_cliente->id, path_archivo);

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

						int pudo_eliminarse = eliminar_archivo_swap(memoria, pcb_proceso);

						if(pudo_eliminarse==0){
							log_info(memoria->memoria_log, "ARCHIVO SWAP DEL PROCESO %d ELIMINADO", pcb_proceso->id);
						}else{
							log_info(memoria->memoria_log, "ARCHIVO SWAP DEL PROCESO %d NO SE PUDO ELIMINAR", pcb_proceso->id);
						}

						//list_destroy(tabla_primer_nivel->entradas);
						//free(tabla_primer_nivel);
						pcb_proceso->tabla_paginas = NULL; //??????????????
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

t_tabla_entradas_primer_nivel* obtener_tabla_primer_nivel_del_proceso(t_pcb* pcb_proceso, t_memoria* memoria){

	int id_tabla_proceso = pcb_proceso->tabla_paginas;

	t_list* tablas_primer_nivel_del_sistema = memoria->tablas_primer_nivel;

	int tamanio_lista_primer_nivel = list_size(tablas_primer_nivel_del_sistema);

	int tabla_actual;
	for(tabla_actual = 0; tabla_actual < tamanio_lista_primer_nivel; tabla_actual++){
		if(list_get(tablas_primer_nivel_del_sistema, tabla_actual)!= NULL){
			t_tabla_entradas_primer_nivel* tabla_primer_nivel_de_la_iteracion = list_get(tablas_primer_nivel_del_sistema, tabla_actual);
			int id_tabla_de_la_iteracion = tabla_primer_nivel_de_la_iteracion->id_tabla;
			if(id_tabla_proceso == id_tabla_de_la_iteracion){
				log_info(memoria->memoria_log, "LA TABLA DE PRIMER NIVEL DEL PROCESO %d es %d", pcb_proceso->id, id_tabla_de_la_iteracion);
				return tabla_primer_nivel_de_la_iteracion;
			}
		}

	}
	log_info(memoria->memoria_log, "NO SE ENCONTRO LA TABLA DE PRIMER NIVEL DEL PROCESO");
	return NULL;

}

