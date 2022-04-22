#include "kernel.h"

int main(int argc, char** argv) {

	char* puerto_kernel;

	t_log* logger;
	t_config* config;

	config = iniciar_config();

	puerto_kernel = config_get_string_value(config, "PUERTO_ESCUCHA");

	logger = log_create("kernel.log", "Kernel", 1, LOG_LEVEL_DEBUG);

	int kernel_fd = start_kernel(logger, "Kernel", puerto_kernel);

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
