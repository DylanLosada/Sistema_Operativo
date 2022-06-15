#include "memoria.h"
#include "funciones_memoria.h"


int main(void) {

//	HACER HANDSHAKE CON CPU DONDE YO PASO cantidad de entradas por tabla de páginas y tamaño de página.Y A MI ME PASAN IP CPU E IP KERNELL

	logger = log_create("memoria.log", "Modulo_Memoria", 1, LOG_LEVEL_DEBUG);
	log_info(logger, "--------------------------------------------\n");

	t_config_memoria* config_memoria = create_config(logger);

	memoria->memoria_log = logger;
	memoria->memoria_config= config_memoria;

    int server_fd = start_memoria(memoria);

    memoria->server_fd = server_fd;

    //Si iniciar memoria falla retorna 0 osea error
    if(!iniciar_memoria_paginada()){
            return 0;
        }

    //Creo un hilo para lo q es manejar conexiones, el otro flujo puede seguir para pedirle cosas a la memoria desde consola
	pthread_t hilo_servidor;
	pthread_create(&hilo_servidor, NULL, manejar_conexion,(void*)memoria);
	pthread_detach(hilo_servidor);

	liberar_conexion(server_fd);
	liberar_memoria();

	return 0;
}

int iniciar_memoria_paginada(){

	int id_pagina = 0;
	logger = memoria->memoria_log;
	int tamanio_memoria = memoria->memoria_config->tamanio_memoria;

	memoriaRAM = malloc(tamanio_memoria);

	log_info(logger,"Direccion inicial de memoria: %d",memoriaRAM);

	if(memoriaRAM == NULL){
	        perror("MALLOC FAIL!\n");
	        return 0;
	    }

    int tamanio_paginas = memoria->memoria_config->tamanio_pagina;
    //char * path_memoria_virtual = config_memoria.path_swap; ver tema de memoria virtual, porq hay tlb y el swap no es con memoria virtual sino con archivos

    cant_frames_principal = tamanio_memoria / tamanio_paginas;

    log_info(logger,"/// Se tienen %d marcos de %d bytes en memoria principal",cant_frames_principal, tamanio_paginas);


    //calloc asigna la memoria solicitada y le devuelve un puntero. La diferencia entre malloc y calloc es que
        //malloc no establece la memoria en cero, mientras que calloc establece la memoria asignada en cero.
    int bitarrayMemoria = calloc(cant_frames_principal, sizeof(int));

    if(bitarrayMemoria == NULL){
        perror("CALLOC FAIL!\n");
        return 0;
    }

    return 1;
}

//----------Tema de creacion de hilos-------------------
void manejar_conexion(void* void_args){
	t_memoria* memoria = (t_memoria*) void_args;
	logger = memoria->memoria_log;
	int server_fd = memoria->server_fd;
	    while(1){
	        int cliente_fd = wait_client(server_fd, logger, "Cliente", "Memoria");
	        pthread_t hilo_servidor;
	        pthread_create (&hilo_servidor, NULL , (void*)administrar_cliente,(void*) cliente_fd);
	        pthread_detach(hilo_servidor);
	    }
	}

//ME QUEDE ACA EL RESTO ESTA JOYA
//SWITCH DEL CODIGO OPERACION
int administrar_cliente(int cliente_fd){
	while(1){
		//ver que el cod op es un int
		t_paquete* paquete_recibido = recibir_operacion(cliente_fd); //Ver esto q hay funciones compartidas
	        switch(paquete_recibido->codigo_operacion){
	        //El switch es por orden del enum!
	            case NEW:
	                iniciar_proceso(cliente_fd);
	                break;
	            default:
	                //log_warning(logger, "Operacion desconocida\n");
	                break;
	        }
	    }
	    return EXIT_SUCCESS;

}
//Administrar proceso tiene que hacer toda la logica, acordate que este socket puede llegar desde kernell o desde cpu
//kernell lo unico que hace es enviar pcbs de procesos nuevos para almacenarlos en memoria
//cpu manda el pcb con cierta instruccion. Dependiendo la instruccion es lo que hay que hacer, en estos casos cpu deberia quedarse esperando a q memoria le mande el ok

//Aca podria recibir el pcb en vez del cliente_fd
void iniciar_proceso(int cliente_fd){
    char* instrucciones;
    int size;

    //buffer seria el mensaje que llegaria de kernell por proceso. Estaria piola q sea id, tamañoProceso, instrucciones

    void* buffer = recibir_buffer(&size,cliente_fd);

    //Ver si hay algo mas q se almacenaria

    int idProceso =leer_entero(buffer,0); //Pos 0 idProceso
    int tamanio_proceso=leer_entero(buffer,1); //Pos 1 tamaño q llega por consola
    instrucciones=leer_string(buffer,2); //Pos 2 instrucciones

    log_info(logger, "Iniciando proceso %d con %c instrucciones que pesa %d...", idProceso, instrucciones, tamanio_proceso);

    int pudeGuardar = guardar_proceso_en_paginacion(idProceso, instrucciones, tamanio_proceso);

    t_paquete * paquete = crear_paquete();
    agregar_entero_a_paquete(paquete, pudeGuardar);

    //Avisa si pudo o no guardar
    if(pudeGuardar == 1){
	    paquete->codigo_operacion = OPERACION_EXITOSA;
        log_info(logger, "----------> Se guarda el proceso [%d] en memoria\n", idProceso);
    }else{
        paquete->codigo_operacion = OPERACION_FALLIDA;
        log_error(logger, "----------> No hay lugar para guardar el proceso [%d] en memoria\n", idProceso);
    }
    enviar_paquete(paquete,cliente_fd);
    eliminar_paquete(paquete);
    free(buffer);
    free(instrucciones);
}

//Tendria que recibir el pcb!
int guardar_proceso_en_paginacion(int idProceso, char * instrucciones, int tamanio_proceso){
	//ceil redondea el double para el prox int
	int paginasNecesarias = ceil((double) tamanio_proceso/ (double) config_memoria.tamanio_pagina);


	// HAY QUE CHEQUEAR NO SOLO QUE HAYA ESPACIO EN MEMORIA DE LAS PAGS SINO Q TAMBIEN LA CANT DE PAGINAS A AGREGAR NO SUPERE EL LIMITE MAX DE PAGS POR PROCESO

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

}

t_list* guardar_proceso(int idProceso, int tamanio_proceso){
    t_list* paginasQueOcupa;
    paginasQueOcupa = guardar_proceso_en_paginas(idProceso, tamanio_proceso);
    return paginasQueOcupa;
}

t_list* guardar_proceso_en_paginas(int idProceso, int tamanio){ //OCUPA PAGINAS NUEVAS PERO NO COLOCO NADA POR AHORA
    t_list* paginasQueOcupa = list_create();
    t_list* framesLibres = buscar_frames_libres_en(MEMORIA_PRINCIPAL);
    int tamanioPag = config_memoria.tamanio_pagina;

    // Si no hay frames en memoria principal, hago swap
    //if(hay frames libres) -> guardar, else -> hacer swap de los bloqueados. pasa q esa orden deberia llegar desde kernell
    
    eliminar_lista(framesLibres);
    return paginasQueOcupa;
}


