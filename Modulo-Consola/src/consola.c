#include "consola.h"

#define NO_OP "NO_OP"

int main(int argc, char** argv)
{
	/*---------------------------------------------------PARTE 2-------------------------------------------------------------*/

	int conexion;
	char* ip;
	char* puerto;
	t_paquete* valor;

	t_log* logger = iniciar_logger();
	t_config* config = iniciar_config();
	log_info(logger, "Preparando la Consola!!!");


	/* ---------------- ARCHIVOS DE CONFIGURACION ---------------- */

	// Usando el config creado previamente, leemos los valores del config y los 
	// dejamos en las variables 'ip', 'puerto' y 'valor'
	puerto = config_get_string_value(config,"PUERTO_KERNEL");
	ip = config_get_string_value(config,"IP_KERNEL");


	log_info(logger, "Leyendo archivo de Pseudocodigo.......");
	// Armamos y enviamos el paquete (depuramos)
	t_paquete* paquete = makePaquete(conexion, argv[2], strtol(argv[1], &argv[1], 10));

	// Creamos una conexión hacia el servidor
	conexion = crear_conexion(ip, puerto);
	enviar_paquete(paquete, conexion);
	eliminar_paquete(paquete);

	terminar_programa(conexion, logger, config);
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

/*void leer_consola(t_log* logger)
{
	char* leido;

	// La primera te la dejo de yapa
	leido = readline("> ");

	// El resto, las vamos leyendo y logueando hasta recibir un string vacío
	while(strcmp(leido, "") != 0){
			log_info(logger, leido);
			free(leido);
			leido = readline("> ");
		}

	// ¡No te olvides de liberar las lineas antes de regresar!
	free(leido);
}*/

t_paquete* makePaquete(int conexion, char* pathFile, int processSize)
{
	char* instructs = string_new();

	generateInstructs(pathFile, &instructs);
	t_paquete* paquete = crear_paquete(processSize);
	agregar_a_paquete(paquete, instructs, sizeof(instructs));
	free(instructs);
	return paquete;
}

void generateInstructs(char* pathFile, char** instructs){
	t_log* logger = iniciar_logger();
    size_t len  = 0;
    FILE* pseudocodeFile = fopen(pathFile, "r");

    while(!feof(pseudocodeFile)){
        char* instructRead = NULL;
        getline(&instructRead, &len, pseudocodeFile);
        strtok(instructRead,"\n");   //con esto borro el \n que se lee
        checkCodeOperation(instructRead, instructs);
    }
    log_info(logger, *instructs);
	fclose(pseudocodeFile);
}

void checkCodeOperation(char* intructrRead, char** instructs){
	char** intructrReadSplitBySpaces = string_split(intructrRead, " ");

	if(strcmp(intructrReadSplitBySpaces[0], NO_OP) == 0){
	  int endCondition = strtol(intructrReadSplitBySpaces[1], &intructrReadSplitBySpaces[1], 10);
	  for(int repeatIntruct = 1; repeatIntruct <= endCondition; repeatIntruct++){
		  string_append_with_format(instructs, "%s|", NO_OP);
	  }
	}else{
		string_append_with_format(instructs, "%s|", intructrRead);
	}
}

void terminar_programa(int conexion, t_log* logger, t_config* config)
{
	/* Y por ultimo, hay que liberar lo que utilizamos (conexion, log y config) 
	  con las funciones de las commons y del TP mencionadas en el enunciado */
	close(conexion);
	log_destroy(logger);
	config_destroy(config);
}
