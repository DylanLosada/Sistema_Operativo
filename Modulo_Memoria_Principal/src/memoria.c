#include "memoria.h"
#include "funciones_memoria.h"

int id_tablas_primer_nivel = 0;
int id_tablas_segundo_nivel = 0;

int main(void) {
	t_memoria* memoria = malloc(sizeof(t_memoria));

	logger = log_create("memoria.log", "Modulo_Memoria", 1, LOG_LEVEL_DEBUG);
	log_info(logger, "--------------------------------------------\n");

	t_config_memoria* config_memoria = create_config(logger);

	memoria->memoria_log = logger;
	memoria->memoria_config= config_memoria;

    int server_fd = start_memoria(memoria);

    memoria->server_fd = server_fd;

    t_list* tablas_primer_nivel = list_create();
    tablas_primer_nivel = memoria->tablas_primer_nivel;

    t_list* tablas_segundo_nivel = list_create();
    tablas_segundo_nivel = memoria->tablas_segundo_nivel;

    int* bitarrayMemoria = iniciar_memoria_paginada(memoria);

    //Creo un hilo para lo q es manejar conexiones, el otro flujo puede seguir para pedirle cosas a la memoria desde consola
	pthread_t hilo_servidor;
	pthread_create(&hilo_servidor, NULL, manejar_conexion,(void*)memoria);
	pthread_join(hilo_servidor, NULL);

	//liberar_conexion(server_fd);
	//liberar_memoria();

	return 0;
}

int* iniciar_memoria_paginada(t_memoria* memoria){

	logger = memoria->memoria_log;
	int tamanio_memoria = memoria->memoria_config->tamanio_memoria;

	memoriaRAM = malloc(tamanio_memoria);

	log_info(logger,"Direccion inicial de memoria: %d",memoriaRAM);

	if(memoriaRAM == NULL){
	        perror("MALLOC FAIL!\n");
	        return 0;
	    }

    int tamanio_paginas = memoria->memoria_config->tamanio_pagina;

    cant_frames_principal = tamanio_memoria / tamanio_paginas;

    log_info(logger,"/// Se tienen %d marcos de %d bytes en memoria principal",cant_frames_principal, tamanio_paginas);


    //calloc asigna la memoria solicitada y le devuelve un puntero. La diferencia entre malloc y calloc es que
        //malloc no establece la memoria en cero, mientras que calloc establece la memoria asignada en cero.
    int* bitarrayMemoria = calloc(cant_frames_principal, sizeof(int));

    if(bitarrayMemoria == NULL){
        perror("CALLOC FAIL!\n");
        exit(1);
    }

    return bitarrayMemoria;
}

//----------Tema de creacion de hilos-------------------
void manejar_conexion(void* void_args){
	t_memoria* memoria = (t_memoria*) void_args;
	logger = memoria->memoria_log;

	pthread_mutex_t* semaforo_conexion = malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(semaforo_conexion, NULL);

	int server_fd = memoria->server_fd;
	    while(1){
	    	t_args_administrar_cliente* args_administrar_cliente = malloc(sizeof(t_args_administrar_cliente));
	    	args_administrar_cliente->semaforo_conexion = semaforo_conexion;
	        int cliente_fd = wait_client(server_fd, logger, "Cliente", "Memoria");
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
	while(1){
		// OP CODE
		recv(cliente_fd, &op_code, sizeof(int), MSG_WAITALL);
		pthread_mutex_lock(args_administrar_cliente->semaforo_conexion);
		op_memoria_message op_code_memori = op_code;

		if(op_code_memori == HANDSHAKE){
			hacer_handshake_con_cpu(cliente_fd, args_administrar_cliente->memoria);
		}else{
			t_pcb* pcb_cliente = deserializate_pcb_memoria(cliente_fd); //ver

			if(op_code_memori == NEW){
				//iniciar_proceso(pcb_cliente, cliente_fd);
			}else if (op_code_memori == DELETE){

			} else {
				log_warning(logger, "Operacion desconocida\n");
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

/*
void iniciar_proceso(t_pcb* pcb_cliente, int cliente_fd){

	int id_proceso = pcb_cliente->id;
    int tamanio_proceso = pcb_cliente->processSize;


    log_info(logger, "Iniciando proceso %d que pesa %d...", id_proceso, tamanio_proceso);

    int pudeGuardar = guardar_proceso_en_paginacion(pcb_cliente);

    //t_cpu_paquete* paquete = crear_paquete();
    //agregar_entero_a_paquete(paquete, pudeGuardar);

    //Avisa si pudo o no guardar
    if(pudeGuardar == 1){
	    paquete->op_code = OPERACION_EXITOSA;
        log_info(logger, "----------> Se guarda el proceso [%d] en memoria\n", id_proceso);
    }else{
        paquete->op_code = OPERACION_FALLIDA;
        log_error(logger, "----------> No hay lugar para guardar el proceso [%d] en memoria\n", id_proceso);
    }
    enviar_paquete(paquete,cliente_fd);
    eliminar_paquete(paquete);
}



//Tendria que recibir el pcb!
int guardar_proceso_en_paginacion(t_pcb* pcb_cliente){

	int tamanio_proceso = pcb_cliente->processSize;
	int cant_marcos = memoria->memoria_config->marcos_proceso;

	int paginas_necesarias = ceil((double) tamanio_proceso/ (double) memoria->memoria_config->tamanio_pagina);

	//obtengo la cantidad de tabla de paginas de segundo nivel necesarias
	//ej: pag_necesarias = 16, marcos_por_proceso= 4 --> cant_tablas #2 = 4 --> cant_entradas #1 = 4
	int cant_tablas_segundo_necesarias = ceil((double) paginas_necesarias / (double) cant_marcos);

	t_tabla_paginas_primer_nivel* tabla_primer_nivel = malloc(sizeof(t_tabla_paginas_primer_nivel*));

	tabla_primer_nivel->id_proceso = pcb_cliente->id;
	tabla_primer_nivel->id_tabla = id_tablas_primer_nivel +1;

	t_list* entadas_tabla_nivel_uno = list_create();

	int paginas_guardadas = 0;

	for(paginas_guardadas = 0; paginas_guardadas < cant_tablas_segundo_necesarias; paginas_guardadas++){

		t_tabla_paginas_segundo_nivel* tabla_segundo_nivel = malloc(sizeof(t_tabla_paginas_segundo_nivel*));
		tabla_segundo_nivel->id_tabla = id_tablas_segundo_nivel +1;

		t_list* paginas_tabla_segundo_nivel = list_create();

		//Ver si paginas guardadas aumenta desp de entrar en el for, sino habria q agregar un +1
		if(paginas_guardadas == cant_tablas_segundo_necesarias){
			int i= 0;
			for(i=0; i < paginas_necesarias; i++){
				int numero_pagina = 0;
				t_pagina_segundo_nivel* pagina_segundo_nivel;
				pagina_segundo_nivel->id_pagina = numero_pagina + 1;
				pagina_segundo_nivel->presencia = 1;
				pagina_segundo_nivel->uso = 1;
				pagina_segundo_nivel->modificado=1; //??
				//pagina_segundo_nivel->frame_principal ??
				//tabla_nivel_dos = agregar pagina
				paginas_necesarias = paginas_necesarias -1;

			}
		}else{
			int i= 0;
			for(i=0; i < cant_marcos; i++){
				int numero_pagina = 0;
				t_pagina_segundo_nivel* pagina_segundo_nivel;
				pagina_segundo_nivel->id_pagina = numero_pagina + 1;
				pagina_segundo_nivel->presencia = 1;
				pagina_segundo_nivel->uso = 1;
				pagina_segundo_nivel->modificado=1; //??
				//pagina_segundo_nivel->frame_principal ??
				//tabla_nivel_dos = agregar pagina
				paginas_necesarias = paginas_necesarias - 1;

			}


		}
}
	//add tabla de segundo nivel a lista de primer nivel
		//lista_tablas_segundo_nivel_del_proceso = agregar
	//add tabla de segundo nivel a lista global de tablas de segundo nivel
		//tablas_segundo_nivel = agregar
	}

	//add tabla de primer nivel a lista global de tablas de primer nivel
	list_add(tablas_primer_nivel, tabla_primer_nivel);
		//tablas_primer_nivel = agregar
	// actualizar pcb



	/*

	if(puedo_guardar_n_paginas(paginasNecesarias)){
		//Tal vez estaria mejor pasar el pcb
	        t_list* paginas_proceso = guardar_proceso(idProceso, tamanio_proceso);
	        void _mostrar_id(t_pagina* pag){
	            log_info(logger, "%d ",pag->id_pagina);
	        }

	        log_info(logger, "Instrucciones del proceso [%d] se guardan en las paginas: ", idProceso);
	        list_map(paginas_proceso, (void*)_mostrar_id);

	        t_pagina* primerPagina = list_get(paginas_instrucciones,0);

	        //Ver tema de calculo de instruccion logica
	        t_tabla_pagina* tablaPrimerNivel = crear_tabla_pagina(idProceso, tamanio_instrucciones);
	        t_tabla_pagina* tablaSegundoNivel = crear_tabla_pagina(idProceso, tamanio_instrucciones);

	        agregar_paginas_a_tablas_proceso(tablaPrimerNivel, tablaSegundoNivel, paginas_instrucciones);

	        //Aca lo q hay q hacer es actualizar el pcb con la pagina del primer nivel
	        t_pcb* pcb = malloc(sizeof(t_pcb));
	        pcb->idProceso = idProceso;
	        pcb->tablaPaginas = dirInstrucciones;
	        guardar_pcb_paginacion(pcb);
	        free(pcb);
	        return 1;
	    }else{
	        return 0;
	    }
	    */

