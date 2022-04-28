#include "kernel.h"

int main(int argc, char** argv) {
	pthread_mutex_t binary = malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(binary, NULL);
	t_queue* cola_pre_pcb = malloc(sizeof(t_queue));
	char* puerto_kernel;
	char* algoritmo_planificacion;

	t_log* logger;
	t_config* config = iniciar_config();

	puerto_kernel = config_get_string_value(config, "PUERTO_ESCUCHA");
	algoritmo_planificacion = config_get_string_value(config, "ALGORITMO_PLANIFICACION");

	logger = iniciar_logger("kernel.log", "Kernel Trace");

	int kernel_fd = start_kernel(logger, "Kernel", puerto_kernel);

	// CREACION DE HILO //
	pthread_t hilo_planificador;
	t_args_planificador* args = malloc(sizeof(t_args_planificador));
	args->cola_pre_pcb = cola_pre_pcb;
	args->tipo_planificador = algoritmo_planificacion;

	// SE PROCESA LA CONEXION //
	pthread_create(&hilo_planificador, NULL, manejador_planificadores, args);
	pthread_detach(hilo_planificador);

	log_info(logger, "Kernel listo para recibir instrucciones");

	while(bind_kernel(logger, "Kernel", kernel_fd));

	release_connection(&kernel_fd);

	//close_program(logger);



	return EXIT_SUCCESS;
}

t_config* iniciar_config(void){
	t_config* nuevo_config = config_create("kernel.config");

	return nuevo_config;
}

/*
void iterator(char* value) {
	log_info(logger,"%s", value);
}
*/
