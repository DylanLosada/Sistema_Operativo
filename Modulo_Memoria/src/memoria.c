#include "memoria.h"
#include "utils.h"


int main(void) {
	//int socketCliente;

//	HACER HANDSHAKE CON CPU DONDE YO PASO cantidad de entradas por tabla de páginas y tamaño de página.Y A MI ME PASAN IP CPU E IP KERNELL

	char* path_config_memoria = "/home/utnso/tp-2022-1c-SanguchitOS-main/Modulo_Memoria/memoria.config";
    funcionando=true;
    t_config_memoria* config = leer_config(path_config_memoria);
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

	logger = iniciar_logger();
	logger = log_create("../memoria.log", "Modulo_Memoria", 0, LOG_LEVEL_DEBUG);
	log_info(logger, "--------------------------------------------\n");

	// Aca se hace la reserva de espacio de memoria
	memoria_principal = malloc(config_memoria.tamanio_memoria);

	log_info(logger,"Direccion inicial de memoria: %d",memoria_principal);


	//La IP de la memoria esta en CPU config, habria q ver como hacer q memoria lo sepa,
	//tal vez hacer una carpeta compartida para todos, como una shared

	//config_memoria.ip = config_get_int_value(config, "IP");
	//int server_fd = iniciar_servidor(config_memoria.ip, config_memoria.puerto);

	//Aca lo que hago es mientras funcionando este en true, se espera al cliente. Hay que ver
	//El tema de semaforos, tal vez esto podria verse ,mejor o utilizar hilos como hago

	//----------Tema de creacion de hilos para que la memoria funcione y atienda de a un proceso a la vez-------------------

//	while (funcionando) {
//		socketCliente = esperar_cliente(server_fd, 5);
//		if (socketCliente == -1)
//			continue;

//			pthread_t hiloProceso;
//			pthread_create(&hiloProceso,NULL,(void*)administrar_Proceso,socketCliente);
//			pthread_detach(hiloProceso);
//	}


//	liberar_conexion(server_fd);

//	return 0;
	}

//void administrar_cliente(int socketCliente){

//}
//Administrar proceso tiene que hacer toda la logica, acordate que este socket puede llegar desde kernell o desde cpu
//kernell lo unico que hace es enviar pcbs de procesos nuevos para almacenarlos en memoria
//cpu manda el pcb con cierta instruccion. Dependiendo la instruccion es lo que hay que hacer, en estos casos cpu deberia quedarse esperando a q memoria le mande el ok

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

int guardar_proceso_en_paginacion(int idProceso, char * instrucciones, int tamanio_proceso){
	int paginasNecesarias = ceil((double) tamanio_proceso/ (double) config_memoria.tamanio_pagina);

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
    }
    eliminar_lista(framesLibres);
    return paginasQueOcupa;
}


