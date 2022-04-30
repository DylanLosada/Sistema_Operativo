#include "consola.h"
#define NO_OP "NO_OP"

int main(int argc, char** argv){
	/*---------------------------------------------------PARTE 2-------------------------------------------------------------*/

	int connection;
	char* ip;
	char* puerto;
	t_log* logger = iniciar_logger();
	t_config* config = iniciar_config();
	log_info(logger, "Preparando la Consola!!!");


	/* ---------------- ARCHIVOS DE CONFIGURACION ---------------- */
	ip = config_get_string_value(config,"IP_KERNEL");
	puerto = config_get_string_value(config,"PUERTO_KERNEL");

	log_info(logger, "Leyendo archivo de Pseudocodigo.......");
	// Armamos y enviamos el paquete (depuramos)
	char* instructions = generateInstructiosnString(argv[2]);

	// Creamos una conexión hacia el servidor
	connection = crear_conexion(ip, puerto);
	send_instructions(instructions, connection, strtol(argv[1], &argv[1], 10));

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

t_config* iniciar_config(void)
{
	t_config* nuevo_config = config_create("consola.config");

	return nuevo_config;
}

char* generateInstructiosnString(char* pathFile)
{
	char* instructs = string_new();
	size_t len  = 0;
	FILE* pseudocodeFile = fopen(pathFile, "r");

	while(!feof(pseudocodeFile)){
		char* instructRead = NULL;
		getline(&instructRead, &len, pseudocodeFile);
		strtok(instructRead,"\n");   //con esto borro el \n que se lee
		checkCodeOperation(instructRead, &instructs);
	}
	fclose(pseudocodeFile);
	return instructs;
}

void appendNoOpToInstructionsString(char** intructrReadSplitBySpaces,char** instructs) {
	int endCondition = strtol(intructrReadSplitBySpaces[1], &intructrReadSplitBySpaces[1], 10);
	for (int repeatIntruct = 1; repeatIntruct <= endCondition; repeatIntruct++) {
		string_append_with_format(instructs, "%s|", NO_OP);
	}
}

void appendOperationToInstructionsString(char* intructrRead, char** instructs) {
	string_append_with_format(instructs, "%s|", intructrRead);
}

void checkCodeOperation(char* instructRead, char** instructs){
	char** intructrReadSplitBySpaces = string_split(instructRead, " ");

	if(strcmp(intructrReadSplitBySpaces[0], NO_OP) == 0){
		appendNoOpToInstructionsString(intructrReadSplitBySpaces, instructs);
	}else{
		appendOperationToInstructionsString(instructRead, instructs);
	}
}

void waitForResponse(int conexion, t_log* logger, t_config* config)
{
	int signal;
	recv(conexion, &signal, sizeof(int), 0);
	log_info(logger, "Proceso finalizado, cerrando consola........");
	close(conexion);
	log_destroy(logger);
	config_destroy(config);
}
