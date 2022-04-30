#include "kernel.h"


int main(int argc, char** argv) {
	pthread_mutex_t binary;
	pthread_mutex_init(&binary, NULL);
	t_queue* cola_pre_pcb = malloc(sizeof(t_queue));
	t_config_kernel* KERNEL_CONFIG = malloc(sizeof(t_config_kernel));
	t_log* kernel_logger;

	kernel_logger = log_create("kernel.log", "Kernel", 1, LOG_LEVEL_DEBUG);

	int config_kernel = create_config(KERNEL_CONFIG, kernel_logger);

	int kernel_fd = start_kernel(KERNEL_CONFIG->PUERTO_ESCUCHA, kernel_logger);

	// CREACION DE HILO //
	pthread_t hilo_planificador;
	t_args_planificador* args = malloc(sizeof(t_args_planificador));
	args->cola_pre_pcb = cola_pre_pcb;
	args->tipo_planificador = KERNEL_CONFIG->ALGORITMO_PLANIFICACION;

	// SE PROCESA LA CONEXION //
	pthread_create(&hilo_planificador, NULL, manejador_planificadores, args);
	pthread_detach(hilo_planificador);

	while(bind_kernel(kernel_fd, kernel_logger));

	release_connection(&kernel_fd);

	//close_program(logger);

	return EXIT_SUCCESS;
}



