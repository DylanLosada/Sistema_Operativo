#include "planificadores.h"

void handler_planners(void* void_args){
	t_args_planificador* args = (t_args_planificador*) void_args;
	pthread_mutex_t* mutex = args->mutex;
	t_config_kernel* config_kernel = args->config_kernel;

	// conexiones a los demas modulos.
	t_sockets_cpu* sockets_cpu = malloc(sizeof(t_sockets_cpu));
	sockets_cpu->interrupt = connect_to_interrupt_cpu(config_kernel);
	sockets_cpu->dispatch= connect_to_dispatch_cpu(config_kernel);
	int socket_kernel_memoria = connect_to_memoria(config_kernel);

	// variables que necesito como int.
	int TIEMPO_MAXIMO_BLOQUEADO = strtol(config_kernel->TIEMPO_MAXIMO_BLOQUEADO, &config_kernel->TIEMPO_MAXIMO_BLOQUEADO, 10);
	int GRADO_MULTIPROGRAMCION = strtol(config_kernel->GRADO_MULTIPROGRAMACION, &config_kernel->GRADO_MULTIPROGRAMACION, 10);
	int ALFA = strtol(config_kernel->ALFA, &config_kernel->ALFA, 10);

	// creo estados
	t_states* states = malloc(sizeof(t_states));
	states->state_new = queue_create();
	states->state_exit = queue_create();
	states->state_ready = list_create();
	states->state_running = queue_create();
	states->state_suspended_blocked = list_create();
	states->state_suspended_ready = list_create();
	states->state_blocked = list_create();

	// ciclo de planificadores.
	while(1){
		long_term_planner(mutex, args->pre_pcbs, states);
		mid_term_planner(TIEMPO_MAXIMO_BLOQUEADO, states);
		short_term_planner(sockets_cpu, config_kernel, ALFA, GRADO_MULTIPROGRAMCION, TIEMPO_MAXIMO_BLOQUEADO, states);
	}
}

bool comparate_time_rafaga(void* previous, void* next){
	t_pcb* prebius_pcb = (t_pcb*)previous;
	t_pcb* next_pcb = (t_pcb*)next;
	return *prebius_pcb->rafaga<=*next_pcb->rafaga;
}

void order_state(t_list* list_to_oreder, int ALFA){
	t_pcb* pcb_anterior = list_get(list_to_oreder, 0);
	for(int index = 1; index < list_size(list_to_oreder); index++){
		t_pcb* pcb_to_calculate_rafaga = list_get(list_to_oreder, index);
		if(!hasCalculateRafaga(pcb_to_calculate_rafaga)){
			calculate_rafaga(ALFA, pcb_anterior, pcb_to_calculate_rafaga);
		}
	}
	list_sort(list_to_oreder, comparate_time_rafaga);
}

bool hasCalculateRafaga(t_pcb* pcb){
	bool hasrafaga = true;

	if(pcb->rafaga == NULL){
		hasrafaga = false;
	}

	return hasrafaga;
}

void calculate_rafaga(int alpha, t_pcb* pcb_anterior, t_pcb* pcb_to_calculate_rafaga){
	int rafagaCalculated = (alpha * pcb_anterior->time_excecuted_rafaga) + ((1 - alpha) * *pcb_anterior->rafaga);
	pcb_to_calculate_rafaga->rafaga = &rafagaCalculated;
}


// La cola de salidas la necesito para poder avisar a cada consola de la finalizacion de su proceso.
void long_term_planner(pthread_mutex_t* mutex, t_queue* pre_pbcs, t_states* states){

	if(queue_size(states->state_exit) > 0){
		close_console_process(states->state_exit);
	}

	add_pcbs_to_new(mutex, pre_pbcs, states->state_new);
}

void mid_term_planner(int TIEMPO_MAXIMO_BLOQUEADO, t_states* states){
	check_time_in_blocked_and_pass_to_suspended_blocked(states->state_suspended_blocked, states->state_blocked, TIEMPO_MAXIMO_BLOQUEADO);
	// de suspended blocked a suspended ready
}

void short_term_planner(t_sockets_cpu* sockets_cpu, t_config_kernel* config_kernel, int ALFA,int GRADO_MULTIPROGRAMCION, int TIEMPO_MAXIMO_BLOQUEADO, t_states* states){
	bool running_state_is_free = hasRunningPcb(states->state_ready);

	int pre_evaluate_add_pcb_to_ready_size = list_size(states->state_ready);
	int empty_space = total_pcbs_short_mid_term(states);
	if(empty_space < GRADO_MULTIPROGRAMCION){

		if(!list_is_empty(states->state_suspended_ready)){
			for(int position_element = 0; position_element < empty_space; position_element++){
				t_pcb* pcb_to_add = list_get(states->state_suspended_ready, position_element);
				if(pcb_to_add != NULL){
					add_pcb_to_state(pcb_to_add, states->state_ready);
				}
			}
		}

		if(total_pcbs_short_mid_term(states) < GRADO_MULTIPROGRAMCION){
			// lugar vacio para los bloquedos, mayor procedencia.
			check_and_update_blocked_to_ready(empty_space, states);
		}

		if(total_pcbs_short_mid_term(states) < GRADO_MULTIPROGRAMCION){
			for(int empty_elem = 0; empty_elem < empty_space; empty_elem++){
				t_pcb* pcb_to_add = queue_pop(states->state_new);
				if(pcb_to_add != NULL){
					add_pcb_to_state(pcb_to_add, states->state_ready);
				}
			}
		}
	}

	if(pre_evaluate_add_pcb_to_ready_size != list_size(states->state_ready) && config_kernel->ALGORITMO_PLANIFICACION == SRT){
		// desalojamos al proceso en CPU.
		if(running_state_is_free){
			t_pcb* pcb_excecuted = queue_pop(states->state_running);
			// pcb_excecuted->time_blocked = clock();
			running_state_is_free = interrupt_cpu(sockets_cpu->interrupt, INTERRUPT, pcb_excecuted);
			list_add_in_index(states->state_ready, 0, pcb_excecuted);
		}

		order_state(states->state_ready, config_kernel);
	}

	// es responsabilidad de la CPU enviarnos el clock del proceso, la siguiente instruccion y.
	send_pcb_to_cpu(list_remove(states->state_ready, 0), sockets_cpu->dispatch);
}

void check_and_update_blocked_to_ready(int empty_space, t_states* states){
	for(int empty_elem = 0; empty_elem < empty_space; empty_elem++){
		t_pcb* pcb_to_add = list_get(states->state_blocked, empty_elem);
		if(pcb_to_add != NULL && pcb_to_add->time_io <= pcb_to_add->time_blocked){
			add_pcb_to_state(pcb_to_add, states->state_ready);
		}
	}
}

bool hasRunningPcb(t_queue* state_ready){
	return !queue_is_empty(state_ready);
}

void check_time_in_blocked_and_pass_to_suspended_blocked(t_list* state_suspended_blocked, t_list* state_blocked, int TIEMPO_MAXIMO_BLOQUEADO){
	t_list* pos_pcbs_with_time_out = list_create();

	for(int index = 0; index < list_size(state_blocked); index++){
		t_pcb* pcb_blocked = list_get(state_blocked, index);

		if(pcb_blocked->time_blocked >= TIEMPO_MAXIMO_BLOQUEADO){
			add_pcb_to_suspended_blocked(pcb_blocked, state_suspended_blocked);
			// enviamos mensaje a memoria y esperamos confirmacion.
			list_add(pos_pcbs_with_time_out, index);
		}
	}

	for(int index = 0; index < list_size(pos_pcbs_with_time_out); index++){
		list_remove(state_blocked, list_get(pos_pcbs_with_time_out, index));
	}
}

void add_pcb_to_suspended_blocked(t_pcb* pcb_blocked, t_list* state_suspended_blocked){
	list_add(state_suspended_blocked, pcb_blocked);
}

void close_console_process(t_queue* state_exit){
	int size_exit_state = queue_size(state_exit);
	for(int elem_destroy = 0; elem_destroy < size_exit_state; elem_destroy++){
		t_pcb* pcb_to_deleat = queue_pop(state_exit);
		// avisar a memoria que se eleimina el pcb.
		close(pcb_to_deleat->id);
		free(pcb_to_deleat->instrucciones);
		free(pcb_to_deleat->program_counter);
		free(pcb_to_deleat->rafaga);
		free(pcb_to_deleat->rafaga);
		free(pcb_to_deleat);
	}
}

int total_pcbs_short_mid_term(t_states* states){
	return list_size(states->state_ready) + list_size(states->state_blocked) + queue_size(states->state_running);
}

void add_pcbs_to_new(pthread_mutex_t* mutex, t_queue* pre_pbcs, t_list* state_new){
	int queue_pre_pcbs_size = queue_size(pre_pbcs);
	for(int position_element = 0; position_element < queue_pre_pcbs_size; position_element++){
		pthread_mutex_lock(mutex);
		t_pcb* pcb = create_pcb(queue_pop(pre_pbcs));
		pthread_mutex_unlock(mutex);
		add_pcb_to_state(pcb, state_new);
	}
}

void add_pcb_to_state(t_pcb* pcb, t_list* state){
	list_add(state, pcb);
}

t_pcb* create_pcb(t_pre_pcb* pre_pcb){
	t_pcb* pcb = malloc(sizeof(t_pcb));
	// aca va la conexion con memoria.
	pcb->id = pre_pcb->pcb_id;
	pcb->instrucciones = pre_pcb->instructions;
	*pcb->program_counter = 0;
	pcb->tabla_paginas = NULL;
	*pcb->rafaga = pre_pcb->initial_burst;
	pcb->processSize = pre_pcb->processSize;
	pcb->time_blocked = 0;
	return pcb;
}

void send_pcb_to_cpu(t_pcb* pcb , int socket_cpu_dispatch){
	if(pcb != NULL){
		t_cpu_paquete* paquete = malloc(sizeof(t_cpu_paquete));
		void* pcb_serializate = serializate_pcb(pcb, paquete);
		send_data_to_server(socket_cpu_dispatch, pcb_serializate, paquete->buffer->size + sizeof(int) + sizeof(int));
	}
}

void* serializate_pcb(t_pcb* pcb, t_cpu_paquete* paquete){
	t_buffer* buffer = malloc(sizeof(t_buffer));

	// Primero completo la estructura buffer interna del paquete.
	buffer->size = sizeof(int) + sizeof(int) + sizeof(t_list) + sizeof(int) + sizeof(int) + sizeof(int) + sizeof(clock_t) + sizeof(int);
	buffer->stream = malloc(buffer->size);
	int offset = 0; // Desplazamiento

	memcpy(buffer->stream, &pcb->id, sizeof(int));
	offset += sizeof(int);
	memcpy(buffer->stream + offset, &pcb->processSize, sizeof(int));
	offset += sizeof(int);
	memcpy(buffer->stream + offset, pcb->program_counter, sizeof(int));
	offset += sizeof(int);
	memcpy(buffer->stream + offset, pcb->tabla_paginas, sizeof(int));
	offset += sizeof(int);
	memcpy(buffer->stream + offset, pcb->rafaga, sizeof(int));
	offset += sizeof(int);
	memcpy(buffer->stream + offset, &pcb->time_io, sizeof(int));
	offset += sizeof(int);
	memcpy(buffer->stream + offset, &pcb->time_excecuted_rafaga, sizeof(int));
	offset += sizeof(int);
	memcpy(buffer->stream + offset, &pcb->time_blocked, sizeof(clock_t));
	offset += sizeof(clock_t);
	memcpy(buffer->stream + offset, &pcb->instrucciones, sizeof(t_list));
	offset += sizeof(t_list);

	// Segundo: completo el paquete.
	paquete->op_code = DISPATCH;
	paquete->buffer = buffer;

	void* a_enviar = malloc(buffer->size + sizeof(int) + sizeof(int));
	offset = 0;

	memcpy(a_enviar, &paquete->op_code, sizeof(int));
	offset += sizeof(int);
	memcpy(a_enviar + offset, &paquete->buffer->size, sizeof(int));
	offset += sizeof(int);
	memcpy(a_enviar + offset, paquete->buffer->stream, paquete->buffer->size);
	offset += paquete->buffer->size;

	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
	return a_enviar;
}

int interrupt_cpu(int socket_kernel_interrupt_cpu, op_code INTERRUPT, t_pcb* pcb_excecuted){
	int confimation_interrupt;
	send_data_to_server(socket_kernel_interrupt_cpu, &INTERRUPT, sizeof(int));
	// esperamos a que la CPU desaloje el proceso y nos avise.
	clock_t time_excecute_pcb = malloc(sizeof(clock_t));
	int count_instructions_excecute;
	recv(socket_kernel_interrupt_cpu, &confimation_interrupt, sizeof(int), 0);
	if(confimation_interrupt > 0){
		recv(socket_kernel_interrupt_cpu, &time_excecute_pcb, sizeof(clock_t), 0);
		recv(socket_kernel_interrupt_cpu, &count_instructions_excecute, sizeof(int), 0);

		pcb_excecuted->time_excecuted_rafaga += time_excecute_pcb;
		// La CPU me retorna cuantas instrucciones logro correr, asi las puedo sacar de la lista de instrucciones
		// y dejo la siguiente instruccion lista para correr.
		list_take_and_remove(pcb_excecuted->instrucciones, count_instructions_excecute);
	}
	return confimation_interrupt;
}

int connect_to_interrupt_cpu(t_config_kernel* config_kernel){
	return create_client_connection(config_kernel->IP_CPU, config_kernel->PUERTO_CPU_INTERRUPT);
}

int connect_to_dispatch_cpu(t_config_kernel* config_kernel){
	return create_client_connection(config_kernel->IP_CPU, config_kernel->PUERTO_CPU_DISPATCH);
}

int connect_to_memoria(t_config_kernel* config_kernel){
	return create_client_connection(config_kernel->IP_MEMORIA, config_kernel->PUERTO_MEMORIA);
}
