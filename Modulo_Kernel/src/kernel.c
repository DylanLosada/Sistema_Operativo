#include "kernel.h"


int main(int argc, char** argv) {
	pthread_mutex_t* mutex;
	pthread_mutex_init(&mutex, NULL);

	t_kernel* kernel = malloc(sizeof(t_kernel));
	t_log* kernel_logger = log_create("kernel.log", "Kernel", 1, LOG_LEVEL_DEBUG);
	t_queue* cola_pre_pcb = malloc(sizeof(t_queue));

	kernel->kernel_log = kernel_logger;

	kernel->kernel_config = malloc(sizeof(t_config_kernel));

	kernel->kernel_config = create_config(kernel_logger);

	kernel->kernel_socket = start_kernel(kernel);


	t_args_planificador* args_planificador = malloc(sizeof(t_args_planificador));

	log_info(kernel_logger, "Grado: %s",kernel->kernel_config->GRADO_MULTIPROGRAMACION);

	args_planificador->pre_pcbs = malloc(sizeof(t_queue));
	args_planificador->pre_pcbs = cola_pre_pcb;
	args_planificador->multiprogramming_degree = strtol(kernel->kernel_config->GRADO_MULTIPROGRAMACION, &kernel->kernel_config->GRADO_MULTIPROGRAMACION, 10);
	args_planificador->mutex = mutex;

	pthread_t hilo_planificador;

	// SE PROCESA LA CONEXION //
	pthread_create(&hilo_planificador, NULL, handler_planners, args_planificador);
	pthread_detach(hilo_planificador);

	t_process_conexion* process_conecction = malloc(sizeof(t_process_conexion));
	process_conecction->cola_pre_pcb = malloc(sizeof(t_queue));
	process_conecction->cola_pre_pcb = cola_pre_pcb;
	process_conecction->kernel = kernel;
	process_conecction->semaforo = mutex;

	while(bind_kernel(kernel, process_conecction));

	//release_connection(&kernel_fd);

	//close_program(logger);

	return EXIT_SUCCESS;
}



