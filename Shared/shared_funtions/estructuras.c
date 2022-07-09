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

	free(logger);
	return;
}

t_pcb* deserializate_pcb_memoria(int socket){
	t_buffer* buffer = malloc(sizeof(t_buffer));
	t_pcb* pcb = malloc(sizeof(t_pcb));
	pcb->tabla_paginas = malloc(sizeof(int));
	pcb->time_blocked = malloc(sizeof(clock_t));
	pcb->instrucciones = list_create();
	int offset = 0;

	// TAMAÑO STREAM
	recv(socket, &buffer->size, sizeof(int), MSG_WAITALL);

	buffer->stream = malloc(buffer->size);
	// STREAM
	recv(socket, buffer->stream, buffer->size, MSG_WAITALL);

	// ID
	memcpy(&pcb->id, buffer->stream, sizeof(int));
	buffer->stream += sizeof(int);


	// processSize
	memcpy(&pcb->processSize, buffer->stream, sizeof(int));
	buffer->stream += sizeof(int);


	// program_counter
	memcpy(&pcb->program_counter, buffer->stream, sizeof(int));
	buffer->stream += sizeof(int);


	// tabla_paginas
	memcpy(&pcb->tabla_paginas, buffer->stream, sizeof(int));
	buffer->stream += sizeof(int);


	// rafaga
	memcpy(&pcb->rafaga, buffer->stream, sizeof(int));
	buffer->stream += sizeof(int);


	// time_io
	memcpy(&pcb->time_io, buffer->stream, sizeof(int));
	buffer->stream += sizeof(int);


	// time_excecuted_rafaga
	memcpy(&pcb->time_excecuted_rafaga, buffer->stream, sizeof(int));
	buffer->stream += sizeof(int);


	// time_blocked
	memcpy(&pcb->time_blocked, buffer->stream, sizeof(clock_t));
	buffer->stream += sizeof(clock_t);


	// (cant instrucciones)
	int cant;
	memcpy(&cant, buffer->stream, sizeof(int));
	buffer->stream += sizeof(int);



	for (int i = 0; i < cant; i++) {
		int size;
		memcpy(&size, buffer->stream, sizeof(int));
		buffer->stream += sizeof(int);

		char* instruccion = malloc(size);
		memcpy(instruccion, buffer->stream, size);
		buffer->stream += size;

		list_add(pcb->instrucciones, instruccion);
	}

	return pcb;
}

t_pcb* deserializate_pcb(int socket, int* op_code){
	t_buffer* buffer = malloc(sizeof(t_buffer));
	t_pcb* pcb = malloc(sizeof(t_pcb));
	pcb->tabla_paginas = malloc(sizeof(int));
	pcb->time_blocked = malloc(sizeof(clock_t));
	pcb->instrucciones = list_create();
	int offset = 0;

	// OP CODE
	recv(socket, op_code, sizeof(int), MSG_WAITALL);

	// TAMAÑO STREAM
	recv(socket, &buffer->size, sizeof(int), MSG_WAITALL);

	buffer->stream = malloc(buffer->size);
	// STREAM
	recv(socket, buffer->stream, buffer->size, MSG_WAITALL);

	// ID
	memcpy(&pcb->id, buffer->stream, sizeof(int));
	buffer->stream += sizeof(int);


	// processSize
	memcpy(&pcb->processSize, buffer->stream, sizeof(int));
	buffer->stream += sizeof(int);


	// program_counter
	memcpy(&pcb->program_counter, buffer->stream, sizeof(int));
	buffer->stream += sizeof(int);


	// tabla_paginas
	memcpy(&pcb->tabla_paginas, buffer->stream, sizeof(int));
	buffer->stream += sizeof(int);

	// rafaga
	memcpy(&pcb->rafaga, buffer->stream, sizeof(int));
	buffer->stream += sizeof(int);


	// time_io
	memcpy(&pcb->time_io, buffer->stream, sizeof(int));
	buffer->stream += sizeof(int);


	// time_excecuted_rafaga
	memcpy(&pcb->time_excecuted_rafaga, buffer->stream, sizeof(int));
	buffer->stream += sizeof(int);


	// time_blocked
	memcpy(&pcb->time_blocked, buffer->stream, sizeof(clock_t));
	buffer->stream += sizeof(clock_t);


	// (cant instrucciones)
	int cant;
	memcpy(&cant, buffer->stream, sizeof(int));
	buffer->stream += sizeof(int);



	for (int i = 0; i < cant; i++) {
		int size;
		memcpy(&size, buffer->stream, sizeof(int));
		buffer->stream += sizeof(int);

		char* instruccion = malloc(size);
		memcpy(instruccion, buffer->stream, size);
		buffer->stream += size;

		list_add(pcb->instrucciones, instruccion);
	}

	return pcb;
}

void* serializate_pcb(t_pcb* pcb, t_cpu_paquete* paquete, int MENSSAGE){
	paquete->buffer = malloc(sizeof(t_buffer));
	loggear_pcb(pcb);

	const int pcb_list_size = list_size(pcb->instrucciones);
	int size = 0;

	for (int i = 0; i < pcb_list_size; i++){
		char* elem = list_get(pcb->instrucciones, i);
		size += strlen(elem) + 1 + sizeof(int); // +1 por fin de string.
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
	memcpy(buffer->stream + offset, &pcb->tabla_paginas, sizeof(int));
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

		int elem_size = strlen(elem) + 1; // +1 por fin de string.
		memcpy(buffer->stream + offset, &elem_size, sizeof(int));
		offset += sizeof(int);

		memcpy(buffer->stream + offset, elem, elem_size);
		offset += elem_size;
	}

	// Segundo: completo el paquete.
	paquete->op_code = MENSSAGE;
	paquete->buffer = buffer;

	void* a_enviar = malloc(buffer->size  + sizeof(int) + sizeof(int));
	offset = 0;

	memcpy(a_enviar, &paquete->op_code, sizeof(int));
	offset += sizeof(int);
	memcpy(a_enviar + offset, &buffer->size, sizeof(int));
	offset += sizeof(int);
	memcpy(a_enviar + offset, paquete->buffer->stream, buffer->size);
	offset += paquete->buffer->size;


	return a_enviar;

}

void* serialize_mmu_memoria(t_cpu_paquete* paquete, int tabla_nivel, int entrada_nivel, int MENSSAGE, op_memoria_message ES_MODIFICADO){
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = sizeof(int) // tabla
			+ sizeof(int) // instruccion
			+ sizeof(int); // entrada

	paquete->buffer->stream = malloc(paquete->buffer->size);
	int offset = 0;

	// TABLA SEGUN NIVEL
	memcpy(paquete->buffer->stream, &tabla_nivel, sizeof(int));
	offset += sizeof(int);


	// ENTRADA SEGUN NIVEL
	memcpy(paquete->buffer->stream + offset, &entrada_nivel, sizeof(int));
	offset += sizeof(int);

	// INSTRUCCION
	memcpy(paquete->buffer->stream + offset, &ES_MODIFICADO, sizeof(int));
	offset += sizeof(int);

	// Segundo: completo el paquete.
	paquete->op_code = MENSSAGE;

	void* a_enviar = malloc(paquete->buffer->size  + sizeof(int) + sizeof(int));
	offset = 0;

	memcpy(a_enviar, &paquete->op_code, sizeof(int));
	offset += sizeof(int);
	memcpy(a_enviar + offset, &paquete->buffer->size, sizeof(int));
	offset += sizeof(int);
	memcpy(a_enviar + offset, paquete->buffer->stream, paquete->buffer->size);
	offset += paquete->buffer->size;

	return a_enviar;
}


void deserialize_mmu_memoria(t_administrar_mmu* administrar_mmu, int socket){
	int size;
	recv(socket, &size, sizeof(int), MSG_WAITALL);
	void* stream = malloc(size);
	recv(socket, stream, size, MSG_WAITALL);
	memcpy(&administrar_mmu->tabla_nivel, stream, sizeof(int));
	memcpy(&administrar_mmu->entrada_nivel, stream + sizeof(int), sizeof(int));
	memcpy(&administrar_mmu->instruccion, stream + sizeof(int) + sizeof(int), sizeof(int));
}

void free_serialize(t_cpu_paquete* paquete){
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}
