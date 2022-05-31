#include "memoria.h"
#include "utils.h"


int main(){


	return 0;
}

// ver que es lo que van a mandar del kernell a memoria, lo mejor seria enviar

void almacenar_proceso(int cliente_fd){

	int size;
	void* buffer = recibir_buffer(&size, cliente_fd);
	int idProceso = leer_entero(buffer, 0);
}

int leer_entero(void * buffer, int desplazamiento){
    int leido;
    memcpy(&leido, buffer + desplazamiento * sizeof(int) , sizeof(int));
    return leido;
}

