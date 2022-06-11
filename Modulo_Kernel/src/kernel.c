#include "kernel.h"

int main(void) {
	pthread_mutex_t* mutex = malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(mutex, NULL);
	pthread_mutex_t* long_condition = malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(long_condition,NULL);
	pthread_cond_t* hasNewConsole = malloc(sizeof(pthread_cond_t));
	pthread_cond_init(hasNewConsole, NULL);
	pthread_mutex_lock(hasNewConsole);

	t_kernel* kernel = malloc(sizeof(t_kernel));
	t_log* kernel_logger = log_create("kernel.log", "Kernel", 1, LOG_LEVEL_DEBUG);
	t_queue* cola_pre_pcb = queue_create();

	kernel->kernel_log = kernel_logger;

	kernel->kernel_config = create_config(kernel_logger);

	kernel->kernel_socket = start_kernel(kernel);

	t_args_planificador* args_planificador = malloc(sizeof(t_args_planificador));

	args_planificador->pre_pcbs = malloc(sizeof(t_queue));
	args_planificador->pre_pcbs = cola_pre_pcb;
	args_planificador->config_kernel = kernel->kernel_config;
	args_planificador->mutex = mutex;
	args_planificador->hasNewConsole = hasNewConsole;

	pthread_t hilo_planificador;

	// SE PROCESA LA CONEXION //
	pthread_create(&hilo_planificador, NULL, handler_planners, args_planificador);
	pthread_detach(hilo_planificador);

	t_process_conexion* process_conecction = malloc(sizeof(t_process_conexion));
	process_conecction->cola_pre_pcb = cola_pre_pcb;
	process_conecction->kernel = kernel;
	process_conecction->semaforo = mutex;
	process_conecction->hasNewConsole = hasNewConsole;

	while(bind_kernel(kernel, process_conecction));

	//release_connection(&kernel_fd);

	//close_program(logger);

	return EXIT_SUCCESS;
}



