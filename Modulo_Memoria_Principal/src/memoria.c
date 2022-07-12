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

    int cantidad_de_frames= (memoria->memoria_config->tamanio_memoria)/(memoria->memoria_config->tamanio_pagina);

    inicializar_lista_de_marcos_libres(cantidad_de_frames, memoria);

    memoria->id_tablas_primer_nivel = 0;
    memoria->id_tablas_segundo_nivel = 0;

    int tamanio_memoria = memoria->memoria_config->tamanio_memoria;
    memoria->espacio_memoria = malloc(tamanio_memoria);

    int tamanio_paginas = memoria->memoria_config->tamanio_pagina;
    log_info(memoria->memoria_log,"SE TIENEN %0.f MARCOS DE %d BYTES EN MEMORIA PRINCIPAL", ceil(tamanio_memoria / tamanio_paginas), tamanio_paginas);

    //-------------------CREO DIRECTORIO PARA LOS ARCHIVOS SWAP------------------------------

	int directorio = mkdir(memoria->memoria_config->path_swap, 0777);

	log_info(memoria->memoria_log,"SE CREA EL DIRECTORIO SWAP EN LA RUTA %s", memoria->memoria_config->path_swap);
    //Creo un hilo para lo q es manejar conexiones, el otro flujo puede seguir para pedirle cosas a la memoria desde consola
	pthread_t hilo_servidor;
	pthread_create(&hilo_servidor, NULL, manejar_conexion,(void*)memoria);
	pthread_join(hilo_servidor, NULL);

	//liberar_conexion(server_fd);
	//liberar_memoria();

	return 0;
}

void inicializar_lista_de_marcos_libres(int cantidad_de_frames, t_memoria* memoria){

	t_list* marcos_libres_para_asignar = list_create();

	for(int frame_actual = 0; frame_actual < cantidad_de_frames; frame_actual++){

		t_marco* marco = malloc(sizeof(t_marco));
		marco->numero_marco = frame_actual;
		marco->pagina = NULL;
		list_add(marcos_libres_para_asignar, marco);
	}
	memoria->marcos_libres = marcos_libres_para_asignar;
	log_info(memoria->memoria_log,"SE COLOCAN TODOS LOS MARCOS DE MEMORIA COMO MARCOS LIBRES");
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
		log_info(memoria->memoria_log,"LLEGA UN CLIENTE A MEMORIA...");
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

			int data = leer_memoria(memoria, dir_fisica);
			send(cliente_fd, &data, sizeof(int), 0);
		}
		else if (op_code_memoria == COPY){
			int op_code = OK;
			int direccion_hacia;
			int direccion_desde;
			recv(cliente_fd, &direccion_hacia, sizeof(int), MSG_WAITALL);
			recv(cliente_fd, &direccion_desde, sizeof(int), MSG_WAITALL);

			int valor_copiado = copiar_memoria(memoria, direccion_desde, direccion_hacia);

			log_info(memoria->memoria_log, "SE EJECUTO EL COPIADO DEL VALOR %d DE %d A %d", valor_copiado, direccion_desde, direccion_hacia);
			send(cliente_fd, &op_code, sizeof(int), 0);
		}
		else if (op_code_memoria == WRITE){
			int op_code = OK;
			int direccion;
			int valor;
			recv(cliente_fd, &valor, sizeof(int), MSG_WAITALL);
			recv(cliente_fd, &direccion, sizeof(int), MSG_WAITALL);

			escribir_memoria(memoria, direccion, valor);

			log_info(memoria->memoria_log, "SE EJECUTO LA ESCRITURA EN LA DIR. %d, VALOR: %d", direccion, valor);
			send(cliente_fd, &op_code, sizeof(int), 0);
		}
		else if (op_code_memoria == TABLA_SEGUNDO_NIVEL){
			// ESTA BIEN ESTO
			int marco_to_swap = -1;
			t_administrar_mmu* administrar_mmu = malloc(sizeof(t_administrar_mmu));
			deserialize_mmu_memoria(administrar_mmu, cliente_fd);
			log_info(memoria->memoria_log, "LA TABLA DE PRIMER NIVEL: %d Y LA ENTRADA DE PRIMER NIVEL %d", administrar_mmu->tabla_nivel, administrar_mmu->entrada_nivel);

			int tabla_segundo_nivel = get_tabla_segundo_nivel(memoria, administrar_mmu->tabla_nivel, administrar_mmu->entrada_nivel);

			void* stream = malloc(sizeof(int)*2);
			memcpy(stream, &tabla_segundo_nivel, sizeof(int));
			memcpy(stream + sizeof(int), &marco_to_swap, sizeof(int));
			send(cliente_fd, stream, 2*sizeof(int), 0);
		}
		else if (op_code_memoria == MARCO){
			// ESTA BIEN ESTO
			int marco_to_swap;

			t_administrar_mmu* administrar_mmu = malloc(sizeof(t_administrar_mmu));
			deserialize_mmu_memoria(administrar_mmu, cliente_fd);
			log_info(memoria->memoria_log, "LA TABLA DE SEGUNDO NIVEL: %d Y LA ENTRADA DE SEGUNDO NIVEL %d", administrar_mmu->tabla_nivel, administrar_mmu->entrada_nivel);

			int marco = get_marco(memoria, administrar_mmu->tabla_nivel, administrar_mmu->entrada_nivel, &marco_to_swap, administrar_mmu->instruccion, administrar_mmu->pcb_id);
			void* stream = malloc(sizeof(int)*2);
			memcpy(stream, &marco, sizeof(int));
			memcpy(stream + sizeof(int), &marco_to_swap, sizeof(int));
			send(cliente_fd, stream, 2*sizeof(int), 0);
		}
		// KERNEL
		else{
			t_pcb* pcb_cliente = deserializate_pcb_memoria(cliente_fd);

			if(op_code_memoria == NEW){

				iniciar_proceso(pcb_cliente, cliente_fd, memoria);

			}else if (op_code_memoria == DELETE){

				t_pcb* pcb_actualizado = eliminar_proceso(pcb_cliente, memoria);
				log_info(memoria->memoria_log, "SE ELIMINAN TODAS LAS ESTRUCTURAS DEL PROCESO %d EN MEMORIA", pcb_cliente->id);
				responder_pcb_a_cliente(pcb_actualizado, cliente_fd, DELETE);

			} else if(op_code_memoria == SWAP){

				log_info(memoria->memoria_log, "EMULAMOS EL RETARDO DE SWAP %d SEGUNDOS", memoria->memoria_config->retardo_swap);
				sleep(memoria->memoria_config->retardo_swap/1000);
				hacer_swap_del_proceso(pcb_cliente, memoria);
				log_info(memoria->memoria_log, "SE SE HACE SWAP DEL PROCESO %d PASANDO LAS PAGINAS DE MEMORIA A SU ARCHIVO", pcb_cliente->id);
				responder_pcb_a_cliente(pcb_cliente, cliente_fd, SWAP);

			}else if(op_code_memoria == RE_SWAP){
				sleep(memoria->memoria_config->retardo_swap/1000);
				hacer_reswap_del_proceso(pcb_cliente, memoria);
				log_info(memoria->memoria_log, "SE SE HACE RESWAP DEL PROCESO %d PASANDO LAS PAGINAS DE MEMORIA A SU ARCHIVO", pcb_cliente->id);
				//Es necesario responder?
				responder_pcb_a_cliente(pcb_cliente, cliente_fd, RE_SWAP);

			}else{
				log_warning(memoria->memoria_log, "Operacion desconocida\n");
			}
			//list_destroy(pcb_cliente->instrucciones);
			//free(pcb_cliente);
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
	log_info(memoria->memoria_log, "DATOS ENVIADOS A CPU.");
}

void iniciar_proceso(t_pcb* pcb_cliente, int cliente_fd, t_memoria* memoria){

	int id_proceso = pcb_cliente->id;
    int tamanio_proceso = pcb_cliente->processSize;


    log_info(memoria->memoria_log, "INICIANDO PROCESO %d QUE PESA %d BYTES", id_proceso, tamanio_proceso);

    t_pcb* pcb_actualizado = guardar_proceso_en_paginacion(pcb_cliente, memoria);


    if(pcb_actualizado->tabla_paginas >= 0){
    		responder_pcb_a_cliente(pcb_actualizado , cliente_fd, NEW);
    		log_info(memoria->memoria_log, "SE GUARDA EL PROCESO [%d] EN MEMORIA\n", id_proceso);

    	}else{
    		responder_pcb_a_cliente(pcb_actualizado , cliente_fd, ERROR);
    		log_info(memoria->memoria_log, "NO HAY LUGAR PARA GUARDAR EL PROCESO [%d] EN MEMORIA\n", id_proceso);

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

	char* path_archivo = obtener_path_swap_del_archivo_del_proceso(pcb_cliente->id, memoria);

	FILE* archivo_proceso;

	archivo_proceso = fopen(path_archivo, "wt");
	log_info(memoria->memoria_log, "SE CREA EL ARCHIVO SWAP DEL PROCESO %d EN LA RUTA %s", pcb_cliente->id, path_archivo);

	//_____________________CREACION DE TABLAS______________________________
	int tamanio_proceso = pcb_cliente->processSize;

	int paginas_necesarias = ceil((double) tamanio_proceso / (double) memoria->memoria_config->tamanio_pagina);

	//obtengo la cantidad de tabla de paginas de segundo nivel necesarias ==> CANTIDAD DE ENTRADAS DE TABLA #1
	//ej: pag_necesarias = 16, marcos_por_proceso= 4 --> cant_tablas #2 = 4 --> cant_entradas #1 = 4
	int cant_tablas_segundo_necesarias = ceil(sqrt((double) paginas_necesarias));

	t_tabla_entradas_primer_nivel* tabla_primer_nivel = malloc(sizeof(t_tabla_entradas_primer_nivel));

	tabla_primer_nivel->id_proceso = pcb_cliente->id;
	tabla_primer_nivel->id_tabla = memoria->id_tablas_primer_nivel;
	aumentar_contador_tablas_primer_nivel(memoria);


	tabla_primer_nivel->marcos_usados = list_create();
	tabla_primer_nivel->marcos_libres = list_create();
	reservar_marcos_libres_proceso(memoria, tabla_primer_nivel);



	tabla_primer_nivel->entradas = list_create();


	//Este for crea las tablas de segundo nivel
	for(int tablas_guardadas = 0; tablas_guardadas < cant_tablas_segundo_necesarias; tablas_guardadas++){

		t_tabla_paginas_segundo_nivel* tabla_segundo_nivel = malloc(sizeof(t_tabla_paginas_segundo_nivel));

		tabla_segundo_nivel->id_tabla = memoria->id_tablas_segundo_nivel;
		aumentar_contador_tablas_segundo_nivel(memoria);

		tabla_segundo_nivel->paginas_segundo_nivel = list_create();
		tabla_segundo_nivel->tabla_1er_nivel = tabla_primer_nivel;

		//Este if es para la ultima tabla de segundo nivel en la cual puede o no usar todas sus paginas
		if(tablas_guardadas + 1 == cant_tablas_segundo_necesarias){
			// Solucion para un solo for: // < (tablas_guardadas + 1 == cant_tablas_segundo_necesarias) ? paginas_necesarias : cant_tablas_segundo_necesarias

			int numero_pagina_de_tabla = 0;
			for(int i=0; i < paginas_necesarias; i++){
				t_pagina_segundo_nivel* pagina_segundo_nivel = malloc(sizeof(t_pagina_segundo_nivel));
				pagina_segundo_nivel->marco_usado = NULL;
				pagina_segundo_nivel->id_pagina = numero_pagina_de_tabla;
				numero_pagina_de_tabla++;
				pagina_segundo_nivel->tabla_segundo_nivel = tabla_segundo_nivel->id_tabla;
				pagina_segundo_nivel->presencia = 0;
				pagina_segundo_nivel->uso = 0;
				pagina_segundo_nivel->modificado=0;

				list_add(tabla_segundo_nivel->paginas_segundo_nivel, pagina_segundo_nivel);
				hacer_swap_de_pagina_inicio(tabla_primer_nivel, pagina_segundo_nivel, tabla_segundo_nivel->id_tabla, archivo_proceso, memoria);

				//Para probar cuanto va subiendo tamaño del archivo
				int tamanio = tamanio_actual_del_archivo(archivo_proceso);
				log_info(memoria->memoria_log, "EL ARCHIVO PESA %d BYTES", tamanio);
			}

		} else {
			int numero_pagina_de_tabla = 0;
			for (int pagina = 0; pagina < cant_tablas_segundo_necesarias; pagina++){
				t_pagina_segundo_nivel* pagina_segundo_nivel = malloc(sizeof(t_pagina_segundo_nivel));
				pagina_segundo_nivel->marco_usado = NULL;
				pagina_segundo_nivel->id_pagina = numero_pagina_de_tabla;
				numero_pagina_de_tabla++;
				pagina_segundo_nivel->tabla_segundo_nivel = tabla_segundo_nivel->id_tabla;
				pagina_segundo_nivel->presencia = 0;
				pagina_segundo_nivel->uso = 0;
				pagina_segundo_nivel->modificado=0;

				list_add(tabla_segundo_nivel->paginas_segundo_nivel, pagina_segundo_nivel);
				hacer_swap_de_pagina_inicio(tabla_primer_nivel, pagina_segundo_nivel, tabla_segundo_nivel->id_tabla, archivo_proceso, memoria);
				paginas_necesarias--;

				//Para probar cuanto va subiendo tamaño del archivo
				int tamanio = tamanio_actual_del_archivo(archivo_proceso);
				log_info(memoria->memoria_log, "EL ARCHIVO PESA %d BYTES", tamanio);

			}
		}
		//Aca agregamos el id de la tabla de segundo nivel
		list_add(tabla_primer_nivel->entradas, tabla_segundo_nivel->id_tabla);

		agregar_tabla_de_segundo_nivel_a_memoria(memoria, tabla_segundo_nivel);
	}

	agregar_tabla_de_primer_nivel_a_memoria(memoria, tabla_primer_nivel);

	pcb_cliente->tabla_paginas = tabla_primer_nivel->id_tabla;


	//_________________CERRADO DE ARCHIVO DEL PROCESO_____________________

	fclose(archivo_proceso);
	log_info(memoria->memoria_log, "SE CIERRA EL ARCHIVO SWAP DEL PROCESO %d DE LA RUTA %s", pcb_cliente->id, path_archivo);

	return pcb_cliente;

}

void agregar_tabla_de_primer_nivel_a_memoria(t_memoria* memoria, t_tabla_entradas_primer_nivel* tabla_primer_nivel){
	list_add(memoria->tablas_primer_nivel, tabla_primer_nivel);
}

void agregar_tabla_de_segundo_nivel_a_memoria(t_memoria* memoria, t_tabla_paginas_segundo_nivel* tabla_segundo_nivel){
	list_add(memoria->tablas_segundo_nivel, tabla_segundo_nivel);
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

	// PASAR MARCOS OCUPADOS A LIBRES GLOBALES
	t_tabla_entradas_primer_nivel* tabla_primer_nivel = obtener_tabla_primer_nivel_del_proceso(pcb_proceso, memoria);
	pasar_marco_ocupado_a_marco_libre_global(tabla_primer_nivel, memoria);
	log_info(memoria->memoria_log,"TERMINAMOS DE PASAR TODOS LOS MARCOS OCUPADOS LOCALES DEL PROCESO %d A LIBRES GLOBALES", pcb_proceso->id);

	// PASAR LIBRES A LIBRES GLOBALES
	agregar_frames_libres_del_proceso_a_lista_global(tabla_primer_nivel, memoria);
	log_info(memoria->memoria_log,"TERMINAMOS DE PASAR TODOS LOS MARCOS LIBRES LOCALES DEL PROCESO %d A LIBRES GLOBALES", pcb_proceso->id);

	// ELIMINAR SWAP
	eliminar_archivo_swap(memoria, pcb_proceso);
	log_info(memoria->memoria_log,"ELIMINAMOS ARCHIVO SWAP DEL PROCESO %d", pcb_proceso->id);

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

	int tabla_actual = 0;
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

void reservar_marcos_libres_proceso(t_memoria* memoria, t_tabla_entradas_primer_nivel* tabla_primer_nivel) {
	for (int i = 0; i < memoria->memoria_config->marcos_proceso; i++) {
		t_marco* marco = list_remove(memoria->marcos_libres, 0);
		list_add(tabla_primer_nivel->marcos_libres, marco);
	}
}

