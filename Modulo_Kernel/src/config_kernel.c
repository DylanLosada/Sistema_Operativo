#include "config_kernel.h"

t_config_kernel* create_config(t_log* log){
	t_config* cfg = config_create("/home/utnso/tp-2022-1c-SanguchitOS/Modulo_Kernel/kernel.config");
	t_config_kernel* kernel_config = malloc(sizeof(t_config_kernel));

	if(cfg == NULL){
		log_error(log, "No se ha encontrado kernel.config");
		return NULL;
	}

	kernel_config->IP_MEMORIA = config_get_string_value(cfg, "IP_MEMORIA");
	kernel_config->PUERTO_MEMORIA = config_get_string_value(cfg, "PUERTO_MEMORIA");
	kernel_config->IP_CPU = config_get_string_value(cfg, "IP_CPU");
	kernel_config->PUERTO_CPU_DISPATCH = config_get_string_value(cfg, "PUERTO_CPU_DISPATCH");
	kernel_config->PUERTO_CPU_INTERRUPT = config_get_string_value(cfg, "PUERTO_CPU_INTERRUPT");
	kernel_config->PUERTO_ESCUCHA = config_get_string_value(cfg, "PUERTO_ESCUCHA");
	kernel_config->ALGORITMO_PLANIFICACION = config_get_string_value(cfg, "ALGORITMO_PLANIFICACION");
	kernel_config->ESTIMACION_INICIAL = config_get_string_value(cfg, "ESTIMACION_INICIAL");
	kernel_config->ALFA = config_get_string_value(cfg, "ALFA");
	kernel_config->GRADO_MULTIPROGRAMACION = config_get_string_value(cfg, "GRADO_MULTIPROGRAMACION");
	kernel_config->TIEMPO_MAXIMO_BLOQUEADO = config_get_string_value(cfg, "TIEMPO_MAXIMO_BLOQUEADO");

	log_info(log, "Archivo de configuracion cargado con exito");

	free(cfg);
	return kernel_config;

}
