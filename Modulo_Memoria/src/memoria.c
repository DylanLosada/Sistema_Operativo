#include "memoria.h"
#include "utils.h"


int main(void){
	char* path_config_memoria = "/home/utnso/tp-2022-1c-SanguchitOS-main/Modulo_Memoria/memoria.config";
	t_log* logger = iniciar_logger();
	logger = log_create("../memoria.log", "Modulo_Memoria", 0, LOG_LEVEL_DEBUG);
	log_info(logger, "--------------------------------------------\n");
	t_config* config = leer_config(path_config_memoria);
	config_memoria.puerto = config_get_int_value(config, "PUERTO");
	//La IP de la memoria esta en CPU config, habria q ver como hacer q memoria lo sepa,
	//tal vez hacer una carpeta compartida para todos, como una shared
	//config_memoria.ip = config_get_int_value(config, "IP");

	int server_fd = iniciar_servidor(config_memoria.ip, config_memoria.puerto);

	if(!iniciar_memoria()){
		return 0;
	}


}
