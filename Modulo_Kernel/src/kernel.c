#include "kernel.h"


int main(int argc, char** argv) {
	t_config_kernel* KERNEL_CONFIG;
	t_log* kernel_logger;

	kernel_logger = log_create("kernel.log", "Kernel", 1, LOG_LEVEL_DEBUG);

	int config_kernel = create_config(KERNEL_CONFIG, kernel_logger);

	int kernel_fd = start_kernel(KERNEL_CONFIG->PUERTO_ESCUCHA, kernel_logger);

	/*if(!kernel_fd || !config_kernel){

		return EXIT_FAILURE;
	}*/



	while(bind_kernel(kernel_fd, kernel_logger));

	release_connection(&kernel_fd);

	//close_program(logger);



	return EXIT_SUCCESS;
}



