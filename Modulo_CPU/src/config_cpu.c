#include "config_cpu.h"

t_config_cpu* create_config_cpu(t_log* log){
	t_config* cfg = config_create("/home/utnso/tp-2022-1c-SanguchitOS/Modulo_CPU/cpu.config");
	t_config_cpu* cpu_config = malloc(sizeof(t_config_cpu));

	if(cfg == NULL){
		log_error(log, "No se ha encontrado cpu.config");
		return NULL;
	}

	cpu_config->ENTRADAS_TLB = config_get_string_value(cfg, "ENTRADAS_TLB");
	cpu_config->REEMPLAZO_TLB = config_get_string_value(cfg, "REEMPLAZO_TLB");
	cpu_config->RETARDO_NOOP = config_get_string_value(cfg, "RETARDO_NOOP");
	cpu_config->IP_MEMORIA = config_get_string_value(cfg, "IP_MEMORIA");
	cpu_config->PUERTO_MEMORIA = config_get_string_value(cfg, "PUERTO_MEMORIA");
	cpu_config->PUERTO_ESCUCHA_DISPATCH = config_get_string_value(cfg, "PUERTO_ESCUCHA_DISPATCH");
	cpu_config->PUERTO_ESCUCHA_INTERRUPT = config_get_string_value(cfg, "PUERTO_ESCUCHA_INTERRUPT");
	log_info(log, "Archivo de configuracion cargado con exito");

	return cpu_config;

}
