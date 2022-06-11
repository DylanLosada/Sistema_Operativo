#include "./estructuras.h"

void loggear_pcb(t_pcb* pcb){
	t_log* logger = log_create("shared.log", "Shared", 1, LOG_LEVEL_DEBUG);

	log_info(logger, "\n------PCB Status------\n"
			"ID: %d\n"
			"processSize: %d\n"
			"program_counter: %d\n"
			"tabla_paginas: %d\n"
			"rafaga: %d\n"
			"time_io: %d\n"
			"time_excecuted_rafaga: %d\n"
			"time_blocked: (TODO: clock log)\n"
			"instrucciones:",
			pcb->id, pcb->processSize, pcb->program_counter, pcb->tabla_paginas,
			pcb->rafaga, pcb->time_io, pcb->time_excecuted_rafaga);


	for (int i = 0; i < list_size(pcb->instrucciones); i++){
		char* instruccion = list_get(pcb->instrucciones, i);
		log_info(logger, "%s", instruccion);
	}

	log_info(logger, "%s", "------------------------\n");


	return;
}


t_pcb* deserializate_pcb(t_buffer* buffer){
	t_pcb* pcb = malloc(sizeof(t_pcb));
	int offset = 0;



	// ID
	memcpy(&pcb->id, buffer->stream, sizeof(int));
	offset += sizeof(int);


	// processSize
	memcpy(&pcb->processSize, buffer->stream + offset, sizeof(int));
	offset += sizeof(int);


	// program_counter
	memcpy(&pcb->program_counter, buffer->stream + offset, sizeof(int));
	offset += sizeof(int);


	// tabla_paginas
	memcpy(pcb->tabla_paginas, buffer->stream + offset, sizeof(int));
	offset += sizeof(int);


	// rafaga
	memcpy(&pcb->rafaga, buffer->stream + offset, sizeof(int));
	offset += sizeof(int);


	// time_io
	memcpy(&pcb->time_io, buffer->stream + offset, sizeof(int));
	offset += sizeof(int);


	// time_excecuted_rafaga
	memcpy(&pcb->time_excecuted_rafaga, buffer->stream + offset, sizeof(int));
	offset += sizeof(int);


	// time_blocked
	memcpy(&pcb->time_blocked, buffer->stream + offset, sizeof(clock_t));
	offset += sizeof(clock_t);


	// (cant instrucciones)
	int cant;
	memcpy(&cant, buffer->stream + offset, sizeof(int));
	offset += sizeof(int);


	pcb->instrucciones = list_create();


	for (int i = 0; i < cant; i++) {
		int size;
		memcpy(&size, buffer->stream + offset, sizeof(int));
		offset += sizeof(int);

		char* instruccion = malloc(size); // TODO: Checkear free
		memcpy(instruccion, buffer->stream + offset, size);
		offset += size;


		list_add(pcb->instrucciones, instruccion);
	}




	return pcb;
}

void* serializate_pcb(t_pcb* pcb, t_cpu_paquete* paquete, int MENSSAGE){

	loggear_pcb(pcb);

	const int pcb_list_size = list_size(pcb->instrucciones);
	int size = 0;
	int* tabla_paginas = malloc(sizeof(int));



	for (int i = 0; i < pcb_list_size; i++){
		char* elem = list_get(pcb->instrucciones, i);
		size += string_length(elem) + 1 + sizeof(int); // +1 por fin de string.
	}





	t_buffer* buffer = malloc(sizeof(t_buffer));


	// Primero completo la estructura buffer interna del paquete.
	buffer->size =
			sizeof(int) 		// ID
			+ sizeof(int) 		// processSize
			+ sizeof(int)		// program_counter
			+ sizeof(int) 		// tabla_paginas
			+ sizeof(int) 		// rafaga
			+ sizeof(int) 		// time_io
			+ sizeof(int)		// time_excecuted_rafaga
			+ sizeof(clock_t) 	// time_blocked
			+ sizeof(int)		// (cant instrucciones)
			+ size;				// (largo inst + inst) x cada inst.



	buffer->stream = malloc(buffer->size);
	int offset = 0;



	// ID
	memcpy(buffer->stream, &pcb->id, sizeof(int));
	offset += sizeof(int);


	// processSize
	memcpy(buffer->stream + offset, &pcb->processSize, sizeof(int));
	offset += sizeof(int);


	// program_counter
	memcpy(buffer->stream + offset, &pcb->program_counter, sizeof(int));
	offset += sizeof(int);


	// tabla_paginas
	if(pcb->tabla_paginas != NULL){
		tabla_paginas = pcb->tabla_paginas;
	}
	memcpy(buffer->stream + offset, tabla_paginas, sizeof(int));
	offset += sizeof(int);


	// rafaga
	memcpy(buffer->stream + offset, &pcb->rafaga, sizeof(int));
	offset += sizeof(int);


	// time_io
	memcpy(buffer->stream + offset, &pcb->time_io, sizeof(int));
	offset += sizeof(int);


	// time_excecuted_rafaga
	memcpy(buffer->stream + offset, &pcb->time_excecuted_rafaga, sizeof(int));
	offset += sizeof(int);


	// time_blocked
	memcpy(buffer->stream + offset, &pcb->time_blocked, sizeof(clock_t));
	offset += sizeof(clock_t);


	// (cant instrucciones)
	memcpy(buffer->stream + offset, &pcb_list_size, sizeof(int));
	offset += sizeof(int);


	// instrucciones
	for (int i = 0; i < pcb_list_size; i++){
		char* elem = list_get(pcb->instrucciones, i);

		int elem_size = string_length(elem) + 1; // +1 por fin de string.
		memcpy(buffer->stream + offset, &elem_size, sizeof(int));
		offset += sizeof(int);

		memcpy(buffer->stream + offset, elem, elem_size);
		offset += elem_size;
	}



	// Segundo: completo el paquete.
	paquete->op_code = MENSSAGE;
	paquete->buffer = buffer;

	void* a_enviar = malloc(buffer->size + sizeof(int) + sizeof(int));
	offset = 0;

	memcpy(a_enviar, &paquete->op_code, sizeof(int));
	offset += sizeof(int);
	memcpy(a_enviar + offset, &paquete->buffer->size, sizeof(int));
	offset += sizeof(int);
	memcpy(a_enviar + offset, paquete->buffer->stream, paquete->buffer->size);
	offset += paquete->buffer->size;




	// TEST DESERIALIZE
	//t_pcb* pcb2 = deserializate_pcb(buffer);
	//loggear_pcb(pcb2);


	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);

	// TODO: Free PCB?


	return a_enviar;

}
