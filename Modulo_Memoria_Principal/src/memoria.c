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
    log_info(memoria->memoria_log,"Se tienen %0.f MARCOS de %d BYTES en MEMORIA PRINCIPAL", ceil(tamanio_memoria / tamanio_paginas), tamanio_paginas);

    //-------------------CREO DIRECTORIO PARA LOS ARCHIVOS SWAP------------------------------

	mkdir(memoria->memoria_config->path_swap, 0777);

	log_info(memoria->memoria_log,"SWAP: directorio creado en %s", memoria->memoria_config->path_swap);
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
		log_info(memoria->memoria_log,"REQUEST: LLEGA UN CLIENTE");
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


		// Pedidos de CPU:
		else if (op_code_memoria == READ){
			int dir_fisica;
			recv(cliente_fd, &dir_fisica, sizeof(int), MSG_WAITALL);

			sleep(memoria->memoria_config->retardo_memoria/1000);
			int data = leer_memoria(memoria, dir_fisica);
			log_info(memoria->memoria_log, "READ (direccion % d): VALOR %d. Retardo simulado.\n", dir_fisica, data);

			send(cliente_fd, &data, sizeof(int), 0);
		}
		else if (op_code_memoria == COPY){
			int op_code = OK;
			int direccion_hacia;
			int direccion_desde;
			recv(cliente_fd, &direccion_hacia, sizeof(int), MSG_WAITALL);
			recv(cliente_fd, &direccion_desde, sizeof(int), MSG_WAITALL);

			sleep(memoria->memoria_config->retardo_memoria/1000);
			int valor_copiado = copiar_memoria(memoria, direccion_desde, direccion_hacia);
			log_info(memoria->memoria_log, "COPY (direccion %d a %d): VALOR %d. Retardo simulado.\n", direccion_desde, direccion_hacia, valor_copiado);

			send(cliente_fd, &op_code, sizeof(int), 0);
		}
		else if (op_code_memoria == WRITE){
			int op_code = OK;
			int direccion;
			int valor;
			recv(cliente_fd, &valor, sizeof(int), MSG_WAITALL);
			recv(cliente_fd, &direccion, sizeof(int), MSG_WAITALL);

			sleep(memoria->memoria_config->retardo_memoria/1000);
			escribir_memoria(memoria, direccion, valor);
			log_info(memoria->memoria_log, "WRITE (direccion %d): VALOR %d. Retardo simulado.\n", direccion, valor);

			send(cliente_fd, &op_code, sizeof(int), 0);
		}
		else if (op_code_memoria == TABLA_SEGUNDO_NIVEL){
			int marco_to_swap = -1;
			t_administrar_mmu* administrar_mmu = malloc(sizeof(t_administrar_mmu));
			deserialize_mmu_memoria(administrar_mmu, cliente_fd);

			sleep(memoria->memoria_config->retardo_memoria/1000);

			int tabla_segundo_nivel = get_tabla_segundo_nivel(memoria, administrar_mmu->tabla_nivel, administrar_mmu->entrada_nivel);

			void* stream = malloc(sizeof(int)*2);
			memcpy(stream, &tabla_segundo_nivel, sizeof(int));
			memcpy(stream + sizeof(int), &marco_to_swap, sizeof(int));
			log_info(memoria->memoria_log, "PEDIDO (tabla de segundo nivel %d). Retardo simulado.\n", tabla_segundo_nivel);

			send(cliente_fd, stream, 2*sizeof(int), 0);
		}
		else if (op_code_memoria == MARCO){
			// ESTA BIEN ESTO
			int marco_to_swap;

			t_administrar_mmu* administrar_mmu = malloc(sizeof(t_administrar_mmu));
			deserialize_mmu_memoria(administrar_mmu, cliente_fd);

			sleep(memoria->memoria_config->retardo_memoria/1000);

			int marco = get_marco(memoria, administrar_mmu->tabla_nivel, administrar_mmu->entrada_nivel, &marco_to_swap, administrar_mmu->instruccion, administrar_mmu->pcb_id);
			void* stream = malloc(sizeof(int)*2);
			memcpy(stream, &marco, sizeof(int));
			memcpy(stream + sizeof(int), &marco_to_swap, sizeof(int));
			log_info(memoria->memoria_log, "PEDIDO (numero de marco %d). Retardo simulado.\n", marco);

			send(cliente_fd, stream, 2*sizeof(int), 0);
		}
		// KERNEL
		else{
			t_pcb* pcb_cliente = deserializate_pcb_memoria(cliente_fd);

			if (op_code_memoria == NEW){

				iniciar_proceso(pcb_cliente, cliente_fd, memoria);

			} else if (op_code_memoria == DELETE){
				t_pcb* pcb_actualizado = eliminar_proceso(pcb_cliente, memoria);
				log_info(memoria->memoria_log, "SE ELIMINAN TODAS LAS ESTRUCTURAS Y ARCHIVO SWAP DEL PROCESO %d EN MEMORIA\n", pcb_cliente->id);
			} else if (op_code_memoria == SWAP){
				hacer_swap_del_proceso(pcb_cliente, memoria);
			} else if (op_code_memoria == RE_SWAP){
				hacer_reswap_del_proceso(pcb_cliente, memoria);
			} else {
				log_warning(memoria->memoria_log, "Operacion desconocida\n");
			}

			//free(pcb_cliente);
		}
		pthread_mutex_unlock(args_administrar_cliente->semaforo_conexion);
	}
	    return EXIT_SUCCESS;
}

void hacer_handshake_con_cpu(int cliente_fd, t_memoria* memoria){

	int tamanio_pagina = memoria->memoria_config->tamanio_pagina;
	int entradas_por_tabla = memoria->memoria_config->entradas_por_tabla;

	log_info(memoria->memoria_log, "HANDSHAKE CPU: conectado, realizando handshake.");
	void* a_enviar = malloc(2 * sizeof(int));
	memcpy(a_enviar, &tamanio_pagina, sizeof(int));
	memcpy(a_enviar + sizeof(int), &entradas_por_tabla, sizeof(int));
	send_data_to_server(cliente_fd, a_enviar, 2 * sizeof(int));

	log_info(memoria->memoria_log, "HANDSHAKE CPU: finalizado CORRECTAMENTE.");
}

void iniciar_proceso(t_pcb* pcb_cliente, int cliente_fd, t_memoria* memoria){

	int id_proceso = pcb_cliente->id;
    int tamanio_proceso = pcb_cliente->processSize;


    t_pcb* pcb_actualizado = guardar_proceso_en_paginacion(pcb_cliente, memoria);

    if(pcb_actualizado->tabla_paginas >= 0){
    		responder_pcb_a_cliente(pcb_actualizado , cliente_fd, NEW);
    		log_info(memoria->memoria_log, "PROCESO %d INICIADO y guardado en memoria (%d bytes).", id_proceso, tamanio_proceso);

    	}else{
    		responder_pcb_a_cliente(pcb_actualizado , cliente_fd, ERROR);
    		log_info(memoria->memoria_log, "NO HAY LUGAR PARA GUARDAR EL PROCESO %d EN MEMORIA (%d bytes).", id_proceso, tamanio_proceso);

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
	log_info(memoria->memoria_log, "ARCHIVO CREADO: Proceso %d en RUTA %s", pcb_cliente->id, path_archivo);

	//_____________________CREACION DE TABLAS______________________________
	int tamanio_proceso = pcb_cliente->processSize;

	int paginas_necesarias = ceil((double) tamanio_proceso / (double) memoria->memoria_config->tamanio_pagina);

	// int cant_tablas_segundo_necesarias = ceil(sqrt((double) paginas_necesarias));
	int cant_tablas_segundo_necesarias = memoria->memoria_config->entradas_por_tabla;

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

			}
		}

		// Agregamos el ID de la tabla de segundo nivel.
		list_add(tabla_primer_nivel->entradas, tabla_segundo_nivel->id_tabla);

		agregar_tabla_de_segundo_nivel_a_memoria(memoria, tabla_segundo_nivel);
	}

	agregar_tabla_de_primer_nivel_a_memoria(memoria, tabla_primer_nivel);

	pcb_cliente->tabla_paginas = tabla_primer_nivel->id_tabla;


	//_________________CERRADO DE ARCHIVO DEL PROCESO_____________________

	fclose(archivo_proceso);

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

	// PASAR MARCOS OCUPADOS A LIBRES GLOBALES
	t_tabla_entradas_primer_nivel* tabla_primer_nivel = obtener_tabla_primer_nivel_del_proceso(pcb_proceso, memoria);
	pasar_marco_ocupado_a_marco_libre_global(tabla_primer_nivel, memoria);

	// PASAR LIBRES A LIBRES GLOBALES
	agregar_frames_libres_del_proceso_a_lista_global(tabla_primer_nivel, memoria);

	// ELIMINAR SWAP
	eliminar_archivo_swap(memoria, pcb_proceso);

	return pcb_proceso;
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

