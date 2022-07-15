#include "consola.h"
#define NO_OP "NO_OP"

int main(int argc, char** argv){
	/*---------------------------------------------------PARTE 2-------------------------------------------------------------*/

	int connection;
	char* ip;
	char* puerto;
	t_log* logger = iniciar_logger();
	t_config* config = config_create(argv[3]);

	log_info(logger, "INICIANDO CONSOLA.....");

	/* ---------------- ARCHIVOS DE CONFIGURACION ---------------- */
	ip = config_get_string_value(config,"IP_KERNEL");
	puerto = config_get_string_value(config,"PUERTO_KERNEL");

	log_info(logger, "CONSOLA INICIADA");
	log_info(logger, "LEYENDO ARCHIVO DE PSEUDOCIDIGO.......");
	// Armamos y enviamos el paquete (depuramos)
	log_info(logger, "INSTRUCCIONES CREADAS");
	char* instructions = generateInstructiosnString(argv[2], logger);

	// Creamos una conexión hacia el servidor
	connection = crear_conexion(ip, puerto);
	log_info(logger, "CONSOLA CONECTADA A KERNEL");

	log_info(logger, "INTENTANDO ENVIAR DATA AL KERNEL");
	if(send_instructions(instructions, connection, strtol(argv[1], &argv[1], 10)) < 0){
		error_show("IMPOSIBLE ENVIAR DATA AL KERNEL, CERRANDO CONSOLA.");
		exit(1);
	}
	log_info(logger, "INSTRUCCIONES ENVIADAS CORRECTAMENTE, ESPERANDO FINALIZACION DEL PROCESO.......");

	// Esperamos por la terminacion del proceso
	waitForResponse(connection, logger, config);
}

t_log* iniciar_logger(void)
{
	char* fileLogName = "consola.log";
	char* programName = "Consola Trace";
	t_log_level LEVEL_ENUM = LOG_LEVEL_TRACE;
	t_log* nuevo_logger = log_create(fileLogName, programName, 1, LEVEL_ENUM);

	return nuevo_logger;
}

char* generateInstructiosnString(char* pathFile, t_log* logger)
{
	char* instructs = string_new();
	size_t len  = 0;
	FILE* pseudocodeFile = fopen(pathFile, "r");

	log_info(logger, "-------------------------------");
	while(!feof(pseudocodeFile)){
		char* instructRead = NULL;
		getline(&instructRead, &len, pseudocodeFile);
		strtok(instructRead,"\n");   //con esto borro el \n que se lee
		checkCodeOperation(instructRead, &instructs, logger);
		free(instructRead);
	}
	fclose(pseudocodeFile);
	log_info(logger, "-------------------------------");
	return instructs;
}

void appendNoOpToInstructionsString(char** intructrReadSplitBySpaces,char** instructs, t_log* logger) {
	int endCondition = strtol(intructrReadSplitBySpaces[1], &intructrReadSplitBySpaces[1], 10);
	for (int repeatIntruct = 1; repeatIntruct <= endCondition; repeatIntruct++) {
		string_append_with_format(instructs, "%s|", NO_OP);
		log_info(logger, NO_OP);
	}
}

void appendOperationToInstructionsString(char* intructrRead, char** instructs, t_log* logger) {
	string_append_with_format(instructs, "%s|", intructrRead);
	log_info(logger, intructrRead);
}

void checkCodeOperation(char* instructRead, char** instructs, t_log* logger){
	char** intructrReadSplitBySpaces = string_split(instructRead, " ");

	if(strcmp(intructrReadSplitBySpaces[0], NO_OP) == 0){
		appendNoOpToInstructionsString(intructrReadSplitBySpaces, instructs, logger);
	}else{
		appendOperationToInstructionsString(instructRead, instructs, logger);
	}

	//string_array_destroy(intructrReadSplitBySpaces);
	//free(intructrReadSplitBySpaces);
}

void free_instruction_split(char** intructrReadSplitBySpaces){
	int array_size = string_array_size(intructrReadSplitBySpaces);
	for(int i = array_size - 1; i >= 0; i--){
		free(intructrReadSplitBySpaces[i]);
	}
}

void waitForResponse(int conexion, t_log* logger, t_config* config)
{
	int signal;
	recv(conexion, &signal, sizeof(int), 0);
	log_info(logger, "PROCESO FINALIZANDO, CERRANDO CONSOLA...");
	log_info(logger, "CONSOLA CERRADA.");
	close(conexion);
	log_destroy(logger);
	config_destroy(config);
}
