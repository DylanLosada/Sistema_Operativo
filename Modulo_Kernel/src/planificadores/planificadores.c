#include "planificadores.h"

void handler_planners(void* void_args){
	pthread_mutex_t* mutex_grado_multi = malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(mutex_grado_multi, NULL);
	pthread_mutex_t* hasPcb = malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(hasPcb, NULL);
	pthread_mutex_t* hasPcbBlocked = malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(hasPcbBlocked, NULL);
	pthread_mutex_t* hasPcbRunning = malloc(sizeof(pthread_mutex_t));;
	pthread_mutex_init(hasPcbRunning, NULL);

	pthread_mutex_lock(hasPcb);
	pthread_mutex_lock(hasPcbRunning);
	pthread_mutex_lock(hasPcbBlocked);


	t_monitor_grado_multiprogramacion* monitorGradoMulti = malloc(sizeof(t_monitor_grado_multiprogramacion));
	monitorGradoMulti->gradoMultiprogramacionActual = 0;
	monitorGradoMulti->mutex = mutex_grado_multi;
	bool* isFirstPcb = malloc(sizeof(bool));
	*isFirstPcb = true;

	// genero los estados dependiendo de si son una cola o una lista.
	t_state_queue_hanndler* new = malloc(sizeof(t_state_queue_hanndler));
	t_state_queue_hanndler* exit = malloc(sizeof(t_state_queue_hanndler));
	t_state_queue_hanndler* running = malloc(sizeof(t_state_queue_hanndler));
	t_state_list_hanndler* ready = malloc(sizeof(t_state_list_hanndler));
	t_state_list_hanndler* blocked = malloc(sizeof(t_state_list_hanndler));
	t_state_list_hanndler* suspended_blocked = malloc(sizeof(t_state_list_hanndler));
	t_state_list_hanndler* suspended_ready = malloc(sizeof(t_state_list_hanndler));

	generate_t_queue_state(new);
	generate_t_queue_state(exit);
	generate_t_queue_state(running);
	generate_t_list_state(ready);
	generate_t_list_state(blocked);
	generate_t_list_state(suspended_blocked);
	generate_t_list_state(suspended_ready);

	t_args_planificador* args = (t_args_planificador*) void_args;
	t_config_kernel* config_kernel = args->config_kernel;

	// conexiones a los demas modulos.
	t_sockets_cpu* sockets_cpu = malloc(sizeof(t_sockets_cpu));
	//sockets_cpu->check_state_instructions = connect_to_check_state_instructions_cpu(config_kernel, "9000");
	//sockets_cpu->interrupt = connect_to_interrupt_cpu(config_kernel);
	sockets_cpu->dispatch= connect_to_dispatch_cpu(config_kernel);
	//int socket_kernel_memoria = connect_to_memoria(config_kernel);
	int socket_kernel_memoria = 0;

	// variables que necesito como int.
	char* ALGORITMO_PLANIFICACION = config_kernel->ALGORITMO_PLANIFICACION;
	int TIEMPO_MAXIMO_BLOQUEADO = strtol(config_kernel->TIEMPO_MAXIMO_BLOQUEADO, &config_kernel->TIEMPO_MAXIMO_BLOQUEADO, 10);
	int ESTIMACION_INICIAL = strtol(config_kernel->ESTIMACION_INICIAL, &config_kernel->ESTIMACION_INICIAL, 10);
	int GRADO_MULTIPROGRAMACION = strtol(config_kernel->GRADO_MULTIPROGRAMACION, &config_kernel->GRADO_MULTIPROGRAMACION, 10);
	int ALFA = strtol(config_kernel->ALFA, &config_kernel->ALFA, 10);

	// creo estados
	t_states* states = malloc(sizeof(t_states));
	states->state_new = new;
	states->state_exit = exit;
	states->state_ready = ready;
	states->state_running = running;
	states->state_suspended_blocked = suspended_blocked;
	states->state_suspended_ready = suspended_ready;
	states->state_blocked = blocked;

	// Planificador de largo
	pthread_t hilo_long_term;
	t_args_long_term_planner* args_long_term_planner = malloc(sizeof(t_args_long_term_planner));
	args_long_term_planner->isFirstPcb = isFirstPcb;
	args_long_term_planner->GRADO_MULTIPROGRAMACION = GRADO_MULTIPROGRAMACION;
	args_long_term_planner->ESTIMACION_INICIAL = ESTIMACION_INICIAL;
	args_long_term_planner->monitorGradoMulti = monitorGradoMulti;
	args_long_term_planner->pre_pcbs = args->pre_pcbs;
	args_long_term_planner->pre_pcbs_mutex = args->mutex;
	args_long_term_planner->hasNewConsole = args->hasNewConsole;
	args_long_term_planner->hasPcb = hasPcb;
	args_long_term_planner->socket_memoria = socket_kernel_memoria;
	args_long_term_planner->states = states;
	pthread_create(&hilo_long_term, NULL, long_term_planner, args_long_term_planner);
	pthread_detach(hilo_long_term);

	// Planificador de medio
	pthread_t hilo_mid_term;
	t_args_mid_term_planner* args_mid_term_planner = malloc(sizeof(t_args_mid_term_planner));
	args_mid_term_planner->monitorGradoMulti = monitorGradoMulti;
	args_mid_term_planner->TIEMPO_MAXIMO_BLOQUEADO = TIEMPO_MAXIMO_BLOQUEADO;
	args_mid_term_planner->socket_memoria = socket_kernel_memoria;
	args_mid_term_planner->states = states;
	args_mid_term_planner->hasPcbBlocked = hasPcbBlocked;
	pthread_create(&hilo_mid_term, NULL, mid_term_planner, args_mid_term_planner);
	pthread_detach(hilo_mid_term);

	// Planificador de corto
	pthread_t hilo_short_term;
	t_args_short_term_planner* args_short_term_planner = malloc(sizeof(t_args_short_term_planner));
	args_short_term_planner->monitorGradoMulti = monitorGradoMulti;
	args_short_term_planner->ALFA = ALFA;
	args_short_term_planner->ALGORITMO_PLANIFICACION = ALGORITMO_PLANIFICACION;
	args_short_term_planner->GRADO_MULTIPROGRAMACION = GRADO_MULTIPROGRAMACION;
	args_short_term_planner->TIEMPO_MAXIMO_BLOQUEADO = TIEMPO_MAXIMO_BLOQUEADO;
	args_short_term_planner->sockets_cpu = sockets_cpu;
	args_short_term_planner->socket_memoria = socket_kernel_memoria;
	args_short_term_planner->states = states;
	args_short_term_planner->config_kernel = config_kernel;
	args_short_term_planner->hasPcbBlocked = hasPcbBlocked;
	args_short_term_planner->hasPcb = hasPcb;
	args_short_term_planner->hasPcbRunning = hasPcbRunning;
	pthread_create(&hilo_short_term, NULL, short_term_planner, args_short_term_planner);
	pthread_detach(hilo_short_term);
}

void generate_t_list_state(t_state_list_hanndler* state_hanndler){
	state_hanndler->state = list_create();
	pthread_mutex_t* mutex = malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(mutex, NULL);
	state_hanndler->mutex = mutex;
}

void generate_t_queue_state(t_state_queue_hanndler* state_hanndler){
	state_hanndler->state = queue_create();
	pthread_mutex_t* mutex = malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(mutex, NULL);
	state_hanndler->mutex = mutex;
}

bool comparate_time_rafaga(void* previous, void* next){
	t_pcb* prebius_pcb = (t_pcb*)previous;
	t_pcb* next_pcb = (t_pcb*)next;
	return prebius_pcb->rafaga<=next_pcb->rafaga;
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
	pcb_to_calculate_rafaga->rafaga = (alpha * pcb_anterior->time_excecuted_rafaga) + ((1 - alpha) * pcb_anterior->rafaga);
}

void long_term_planner(void* args_long_term_planner){
	t_args_long_term_planner* args = (t_args_long_term_planner*) args_long_term_planner;

	while(1){
		pthread_mutex_lock(args->hasNewConsole);
		pthread_mutex_lock(args->states->state_exit->mutex);
		if (queue_size(args->states->state_exit->state) > 0) {
			// TODO introducimos logger para avisar que se vana cerrar procesos.
			close_console_process(args->states->state_exit->state, args->socket_memoria);
		}
		pthread_mutex_unlock(args->states->state_exit->mutex);

		add_pcbs_to_new(args->isFirstPcb, args->states, args->pre_pcbs, args->socket_memoria, args->monitorGradoMulti,args->pre_pcbs_mutex, args->GRADO_MULTIPROGRAMACION, args->ESTIMACION_INICIAL);
		pthread_mutex_unlock(args->hasPcb);
	}
}

void mid_term_planner(void* args_mid_term_planner){
	t_args_mid_term_planner* args = (t_args_mid_term_planner*) args_mid_term_planner;

	while (1) {
		if (!list_is_empty(args->states->state_suspended_blocked->state)) {
			pthread_mutex_unlock(args->hasPcbBlocked);
		}

		pthread_mutex_lock(args->states->state_suspended_ready->mutex);
		if (!list_is_empty(args->states->state_blocked->state)) {
			pthread_mutex_unlock(args->hasPcbBlocked);
		}
		pthread_mutex_unlock(args->states->state_suspended_ready->mutex);

		pthread_mutex_lock(args->hasPcbBlocked);
		check_time_in_blocked_and_pass_to_suspended_blocked(args->states->state_suspended_blocked, args->states->state_blocked, args->monitorGradoMulti, args->socket_memoria, args->TIEMPO_MAXIMO_BLOQUEADO);
		check_process_finished_io_and_pass_to_suspended_ready(args->states->state_suspended_blocked, args->states->state_suspended_ready);
		//TODO agregar el binary del corto plazo asi se retroalimentan entre ellos.
	}
}

void short_term_planner(void* args_short_planner){
	t_args_short_term_planner* args = (t_args_short_term_planner*) args_short_planner;

	t_state_list_hanndler* ready = args->states->state_ready;
	t_state_list_hanndler* state_suspended_ready = args->states->state_suspended_ready;
	t_state_queue_hanndler* running = args->states->state_running;
	t_state_queue_hanndler* new = args->states->state_new;
	t_state_queue_hanndler* exit = args->states->state_exit;

	pthread_mutex_t* mutex_check_instruct = malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(mutex_check_instruct, NULL);
	// Hilos de apoyo a los planificadores
	t_args_check_instructions* args_instruction_thread = malloc(sizeof(t_args_check_instructions));
	args_instruction_thread->mutex_check_instruct = mutex_check_instruct;
	args_instruction_thread->hasPcb = args->hasPcb;
	args_instruction_thread->hasPcbRunning = args->hasPcbRunning;
	args_instruction_thread->socket = args->sockets_cpu->check_state_instructions;
	create_check_instructions_thread(args_instruction_thread);

	while(1){
		pthread_mutex_lock(args->states->state_blocked->mutex);
		if(!list_is_empty(args->states->state_blocked->state)){
			pthread_mutex_unlock(args->hasPcbBlocked);
			pthread_mutex_unlock(args->hasPcb);
		}
		pthread_mutex_unlock(args->states->state_blocked->mutex);

		if(!queue_is_empty(running->state)){
			pthread_mutex_unlock(args->hasPcb);
		}

		pthread_mutex_lock(args->hasPcb);
		pthread_mutex_lock(args->states->state_blocked->mutex);
		pthread_mutex_lock(ready->mutex);
		pthread_mutex_lock(args->monitorGradoMulti->mutex);
		bool hasRunning = hasRunningPcb(running->state);
		int pre_evaluate_add_pcb_to_ready_size = list_size(ready->state);
		args->monitorGradoMulti->gradoMultiprogramacionActual = total_pcbs_short_mid_term(args->states);
		pthread_mutex_unlock(ready->mutex);
		pthread_mutex_unlock(args->states->state_blocked->mutex);
		if(args->monitorGradoMulti->gradoMultiprogramacionActual < args->GRADO_MULTIPROGRAMACION){

			pthread_mutex_lock(ready->mutex);
			pthread_mutex_lock(state_suspended_ready->mutex);
			if(!list_is_empty(state_suspended_ready->state)){
				for(int position_element = 0; position_element < abs(args->monitorGradoMulti->gradoMultiprogramacionActual -4); position_element++){
					if(list_size(state_suspended_ready->state) > 0){
						t_pcb* pcb_to_add = list_get(state_suspended_ready->state, position_element);
						if(!has_tabla_paginas(pcb_to_add)){
							pcb_to_add = send_action_to_memoria(pcb_to_add, args->socket_memoria, RE_SWAP);
						}
						if(pcb_to_add != NULL){
							add_pcb_to_state(pcb_to_add, ready->state);
							args->monitorGradoMulti->gradoMultiprogramacionActual++;
						}
					}else{
						break;
					}
				}
			}
			pthread_mutex_unlock(state_suspended_ready->mutex);
			pthread_mutex_unlock(ready->mutex);

			pthread_mutex_lock(ready->mutex);
			pthread_mutex_lock(args->states->state_blocked->mutex);
			if(args->monitorGradoMulti->gradoMultiprogramacionActual < args->GRADO_MULTIPROGRAMACION){
				// lugar vacio para los bloquedos, mayor procedencia.
				check_and_update_blocked_to_ready(&args->monitorGradoMulti->gradoMultiprogramacionActual, args->states);
			}
			pthread_mutex_unlock(args->states->state_blocked->mutex);
			pthread_mutex_unlock(ready->mutex);

			pthread_mutex_lock(ready->mutex);
			pthread_mutex_lock(new->mutex);
			if(args->monitorGradoMulti->gradoMultiprogramacionActual < args->GRADO_MULTIPROGRAMACION && !queue_is_empty(new->state)){
				for(int empty_elem = 0; empty_elem < abs(args->monitorGradoMulti->gradoMultiprogramacionActual -4); empty_elem++){
					if(queue_size(new->state) > 0){
						t_pcb* pcb_to_add = queue_pop(new->state);
						if(pcb_to_add != NULL){
							add_pcb_to_state(pcb_to_add, ready->state);
							args->monitorGradoMulti->gradoMultiprogramacionActual++;
						}
					}else{
						break;
					}
				}
			}
			pthread_mutex_unlock(new->mutex);
			pthread_mutex_unlock(ready->mutex);
		}
		pthread_mutex_unlock(args->monitorGradoMulti->mutex);

		pthread_mutex_lock(ready->mutex);
		pthread_mutex_lock(args_instruction_thread->mutex_check_instruct);
		bool hasNewPcbIntoReady = isNewPcbIntoReady(pre_evaluate_add_pcb_to_ready_size, ready->state);
		//TODO: args_check_instructions->hasUpdateState, lo remplazo por un true momentaneo para evaluar el interrupt
		if(hasRunning && true){
			t_pcb* pcb_excecuted = queue_pop(running->state);
			if(args_instruction_thread->isExitInstruction){
				queue_push(exit->state, pcb_excecuted->id);
			}else{
				pcb_excecuted->time_blocked = clock();
				//pcb_excecuted->time_io = args_instruction_thread->pcb->time_io;
				//pcb_excecuted->time_excecuted_rafaga += args_instruction_thread->pcb->time_excecuted_rafaga;
				//pcb_excecuted->instrucciones = args_instruction_thread->pcb->instrucciones;
				pthread_mutex_lock(args->states->state_blocked->mutex);
				list_add(args->states->state_blocked->state, pcb_excecuted);
				pthread_mutex_unlock(args->states->state_blocked->mutex);
			}
			args_instruction_thread->pcb = NULL;
			hasRunning = false;
		}

		if(hasNewPcbIntoReady && strcmp(args->ALGORITMO_PLANIFICACION, "SRT") == 0){
			// desalojamos al proceso en CPU.
			if(hasRunning){
				t_pcb* pcb_excecuted = queue_pop(running->state);
				/*pcb_excecuted->time_io = args_check_instructions->pcb->time_io;
				pcb_excecuted->time_excecuted_rafaga += args_check_instructions->pcb->time_excecuted_rafaga;
				pcb_excecuted->instrucciones = args_check_instructions->pcb->instrucciones;
				args_check_instructions->pcb = NULL;*/
				// pcb_excecuted->time_blocked = clock();
				// interrupt_cpu(sockets_cpu->interrupt, INTERRUPT, pcb_excecuted);
				list_add_in_index(ready->state, 0, pcb_excecuted);
			}

			order_state(ready->state, args->config_kernel);
		}
		pthread_mutex_unlock(args_instruction_thread->mutex_check_instruct);
		pthread_mutex_unlock(ready->mutex);

		pthread_mutex_lock(ready->mutex);
		// es responsabilidad de la CPU enviarnos el clock del proceso, la siguiente instruccion y.......
		if(!list_is_empty(ready->state) && hasNewPcbIntoReady){
			t_pcb* pcb_ready_to_run = list_remove(ready->state, 0);
			queue_push(running->state, pcb_ready_to_run);
			// introducimos logger para avisar que va aempezar a correr cierto pcb
			send_pcb_to_cpu(pcb_ready_to_run, args->sockets_cpu->dispatch);
			//TODO funcion para liberar memoria del pcb.
			pthread_mutex_unlock(args->hasPcbRunning);
		}else{
			// introducimos logger para avisar que no existen mas pcbs
		}
		pthread_mutex_unlock(ready->mutex);
		pthread_mutex_unlock(args->hasPcbBlocked);
	}
}

bool isNewPcbIntoReady(int pre_evaluate_add_pcb_to_ready_size, t_list* state_ready){
	return pre_evaluate_add_pcb_to_ready_size < list_size(state_ready);
}

void check_and_update_blocked_to_ready(int* empty_space, t_states* states){
	t_list* pos_pcbs_with_complete_io = list_create();

	if(!list_is_empty(states->state_blocked->state)){
		for(int empty_elem = 0; empty_elem < abs(*empty_space -4); empty_elem++){
			t_pcb* pcb_to_add = list_get(states->state_blocked->state, empty_elem);
			int time_blocked = abs(pcb_to_add->time_blocked - clock());
			if(pcb_to_add != NULL && pcb_to_add->time_io <= time_blocked){
				add_pcb_to_state(pcb_to_add, states->state_ready->state);
				*empty_space++;
			}
		}

		for(int index = 0; index < list_size(pos_pcbs_with_complete_io); index++){
			list_add(states->state_ready, list_remove(states->state_blocked, list_get(pos_pcbs_with_complete_io, index)));
		}

		free(pos_pcbs_with_complete_io);
	}
}

bool hasRunningPcb(t_queue* state_running){
	return !queue_is_empty(state_running);
}

void check_process_finished_io_and_pass_to_suspended_ready(t_state_list_hanndler* state_suspended_blocked, t_state_list_hanndler* state_suspended_ready){
	t_list* pos_pcbs_with_io_finished = list_create();

	pthread_mutex_lock(state_suspended_blocked->mutex);
	for(int index = 0; index < list_size(state_suspended_blocked->state); index++){
		t_pcb* pcb_blocked = list_get(state_suspended_blocked->state, index);
		int time_blocked = abs(pcb_blocked->time_blocked - clock());

		// se checkea en short que se haya terminado su tiempo de bloqueado.

		if(time_blocked >= pcb_blocked->time_io){
			add_pcb_to_suspended_blocked(pcb_blocked, state_suspended_ready);
			list_add(pos_pcbs_with_io_finished, index);
		}
	}

	for(int index = 0; index < list_size(pos_pcbs_with_io_finished); index++){
		list_remove(state_suspended_blocked->state, list_get(pos_pcbs_with_io_finished, index));
	}
	pthread_mutex_unlock(state_suspended_blocked->mutex);
}

void check_time_in_blocked_and_pass_to_suspended_blocked(t_state_list_hanndler* state_suspended_blocked, t_state_list_hanndler* state_blocked, t_monitor_grado_multiprogramacion* monitorGradoMulti, int socket_memoria, int TIEMPO_MAXIMO_BLOQUEADO){
	t_list* pos_pcbs_with_time_out = list_create();

	for(int index = 0; index < list_size(state_blocked->state); index++){
		t_pcb* pcb_blocked = list_get(state_blocked->state, index);
		int time_blocked = abs(pcb_blocked->time_blocked - clock());

		// se checkea en short que se haya terminado su tiempo de bloqueado.

		if(time_blocked >= TIEMPO_MAXIMO_BLOQUEADO && time_blocked < pcb_blocked->time_io){
			add_pcb_to_suspended_blocked(pcb_blocked, state_suspended_blocked);
			list_add(pos_pcbs_with_time_out, index);
			pthread_mutex_lock(monitorGradoMulti->mutex);
			monitorGradoMulti->gradoMultiprogramacionActual--;
			pthread_mutex_unlock(monitorGradoMulti->mutex);
		}
	}

	for(int index = 0; index < list_size(pos_pcbs_with_time_out); index++){
		t_pcb* pcb_to_send = list_remove(state_blocked->state, list_get(pos_pcbs_with_time_out, index));
		send_action_to_memoria(pcb_to_send, socket_memoria, FREE_GRADO);
	}
}

void recive_information_from_memoria(t_pcb* pcb, int socket_memoria){
	int* op_code;
	// recibo mensaje y pcb de memoria.
	t_pcb* pcb_received = deserializate_pcb(socket_memoria, op_code);

	if (*op_code == ERROR || *op_code < 0) {
		error_show("OCURRIO UN PROBLEMA INTENTANDO CONECTARSE CON MEMORIA, CODIGO ERROR: %d", *op_code);
		//exit(1);
	} else if (*op_code == CREATE || *op_code == RE_SWAP) {
		pcb->tabla_paginas = pcb_received->tabla_paginas;
	} else if (*op_code == SWAP) {
		// TODO loggeamos que se realizo el SWAP a memoria.
	} else if (*op_code == FREE_GRADO) {
		// TODO loggeamos que se realizo la liberacion de espacio memoria.
	}
}

void add_pcb_to_suspended_blocked(t_pcb* pcb_blocked, t_state_list_hanndler* state_suspended_blocked){
	list_add(state_suspended_blocked, pcb_blocked);
}

void close_console_process(t_queue* state_exit, int socket_memoria){
	int size_exit_state = queue_size(state_exit);
	for(int elem_destroy = 0; elem_destroy < size_exit_state; elem_destroy++){
		t_pcb* pcb_to_deleat = queue_pop(state_exit);
		// avisar a memoria que se eleimina el pcb.
		send_action_to_memoria(pcb_to_deleat, socket_memoria, DELETE);
		close(pcb_to_deleat->id);
		free(pcb_to_deleat->instrucciones);
		free(pcb_to_deleat->program_counter);
		free(pcb_to_deleat->rafaga);
		free(pcb_to_deleat->rafaga);
		free(pcb_to_deleat);
	}
}

int total_pcbs_short_mid_term(t_states* states){
	return list_size(states->state_ready->state) + list_size(states->state_blocked->state) + queue_size(states->state_running->state);
}

void add_pcbs_to_new(bool* isFirstPcb, t_states* states, t_queue* pre_pbcs, int socket_memoria, t_monitor_grado_multiprogramacion* monitorGradoMulti, pthread_mutex_t* mutex, int GRADO_MULTIPROGRAMACION, int ESTIMACION_INICIAL){
	int queue_pre_pcbs_size = queue_size(pre_pbcs);
	for(int position_element = 0; position_element < queue_pre_pcbs_size; position_element++){
		pthread_mutex_lock(mutex);
		t_pcb* pcb = create_pcb(monitorGradoMulti, GRADO_MULTIPROGRAMACION, ESTIMACION_INICIAL, isFirstPcb, queue_pop(pre_pbcs));
		pthread_mutex_unlock(mutex);
		if(pcb->tabla_paginas == NULL){
			// CASO DONDE EL GRADO DE MULTI. NO NOS LO PERMITE
			send_action_to_memoria(pcb, socket_memoria, SWAP);
			pthread_mutex_lock(states->state_suspended_ready->mutex);
			list_add(states->state_suspended_ready->state, pcb);
			pthread_mutex_unlock(states->state_suspended_ready->mutex);
		}else{
			// CASO DONDE EL GRADO DE MULTI. NOS LO PERMITE
			send_action_to_memoria(pcb, socket_memoria, CREATE);
			pthread_mutex_lock(states->state_new->mutex);
			queue_push(states->state_new->state, pcb);
			pthread_mutex_unlock(states->state_new->mutex);
		}
	}
}

void add_pcb_to_state(t_pcb* pcb, t_list* state){
	list_add(state, pcb);
}

t_pcb* create_pcb(t_monitor_grado_multiprogramacion* monitorGradoMulti, int GRADO_MULTIPROGRAMACION, int ESTIMACION_INICIAL, bool* isFirstPcb, t_pre_pcb* pre_pcb){
	t_pcb* pcb = malloc(sizeof(t_pcb));
	// aca va la conexion con memoria.
	pcb->id = pre_pcb->pcb_id;
	pcb->instrucciones = pre_pcb->instructions;
	pcb->program_counter = 0;
	if(*isFirstPcb){
		pcb->rafaga = ESTIMACION_INICIAL;
		*isFirstPcb=false;
	}
	pcb->processSize = pre_pcb->processSize;
	pcb->time_blocked = malloc(sizeof(clock_t));
	pcb->time_io = 0;
	pthread_mutex_lock(monitorGradoMulti->mutex);
	if(monitorGradoMulti->gradoMultiprogramacionActual >= GRADO_MULTIPROGRAMACION || (monitorGradoMulti->gradoMultiprogramacionActual + 1) > GRADO_MULTIPROGRAMACION){
		pcb->tabla_paginas = NULL;
	}else{
		pcb->tabla_paginas = malloc(sizeof(int));
	}
	pthread_mutex_unlock(monitorGradoMulti->mutex);

	return pcb;
}

void send_pcb_to_memoria(t_pcb* pcb , int socket_memoria, op_memoria_message MENSSAGE){
	if(pcb != NULL){
		t_cpu_paquete* paquete = malloc(sizeof(t_cpu_paquete));
		void* pcb_serializate = serializate_pcb(pcb, paquete, MENSSAGE);
		int code_operation = send_data_to_server(socket_memoria, pcb, paquete->buffer + sizeof(int));

		if(code_operation < 0){
			error_show("OCURRIO UN PROBLEMA INTENTANDO CONECTARSE CON MEMORIA, ERROR: IMPOSIBLE CONECTAR");
			//exit(1);
		}
	}
}

void send_pcb_to_cpu(t_pcb* pcb , int socket_cpu_dispatch){
	if(pcb != NULL){
		t_cpu_paquete* paquete = malloc(sizeof(t_cpu_paquete));
		void* pcb_serializate = serializate_pcb(pcb, paquete, DISPATCH);
		send_data_to_server(socket_cpu_dispatch, pcb, paquete->buffer->size + sizeof(int));
	}
}

void update_pcb_with_cpu_data(int socket_kernel_interrupt_cpu, t_pcb* pcb_excecuted){
	// TODO pasarlo a "funcion de deserializacion de Facu"
	int time_excecute_pcb;
	int count_instructions_excecute;
	int time_io;
	recv(socket_kernel_interrupt_cpu, &time_excecute_pcb, sizeof(int), 0);
	recv(socket_kernel_interrupt_cpu, &count_instructions_excecute, sizeof(int), 0);
	recv(socket_kernel_interrupt_cpu, &time_io, sizeof(int), 0);

	pcb_excecuted->time_excecuted_rafaga += time_excecute_pcb;
	pcb_excecuted->time_io = time_io;
	// La CPU me retorna cuantas instrucciones logro correr, asi las puedo sacar de la lista de instrucciones
	// y dejo la siguiente instruccion lista para correr.
	list_take_and_remove(pcb_excecuted->instrucciones, count_instructions_excecute);
}

int interrupt_cpu(int socket_kernel_interrupt_cpu, op_code INTERRUPT, t_pcb* pcb_excecuted){
	send_data_to_server(socket_kernel_interrupt_cpu, &INTERRUPT, sizeof(int));
	// esperamos a que la CPU desaloje el proceso y nos avise.
	update_pcb_with_cpu_data(socket_kernel_interrupt_cpu,pcb_excecuted);
}

int connect_to_interrupt_cpu(t_config_kernel* config_kernel){
	return realize_connection(config_kernel->IP_CPU, config_kernel->PUERTO_CPU_INTERRUPT);
}

int connect_to_check_state_instructions_cpu(t_config_kernel* config_kernel, char* port){
	return realize_connection(config_kernel->IP_CPU, port);
}

int connect_to_dispatch_cpu(t_config_kernel* config_kernel){
	return realize_connection(config_kernel->IP_CPU, config_kernel->PUERTO_CPU_DISPATCH);
}

int realize_connection(char* ip, char* puerto) {
	int connection = 0;
	do{
		connection = create_client_connection(ip,puerto);
	}while(connection <= 0);
	return connection;
}

int connect_to_memoria(t_config_kernel* config_kernel){
	return realize_connection(config_kernel->IP_MEMORIA, config_kernel->PUERTO_MEMORIA);
}

int has_tabla_paginas(t_pcb* pcb_to_add){
	return pcb_to_add->tabla_paginas != NULL;
}

void check_state_of_pcb(void* void_args){
	t_args_check_instructions* args = (t_args_check_instructions*) void_args;
	op_instructions_code code = -1;
	while(1){
		pthread_mutex_lock(args->hasPcbRunning);
		args->pcb = malloc(sizeof(t_pcb));
		args->pcb->time_io = 0;
		args->isExitInstruction = false;
		recv(args->socket, &code, sizeof(int), 0);
		pthread_mutex_lock(args->mutex_check_instruct);
		if(code >= 0){
			update_pcb_with_cpu_data(args->socket,args->pcb);
			if(code == EXIT){
				args->isExitInstruction = true;
			}
		}
		pthread_mutex_unlock(args->mutex_check_instruct);
		pthread_mutex_unlock(args->hasPcb);
	}
}

t_pcb* send_action_to_memoria(t_pcb* pcb, int socket_memoria, int ACTION){
	send_pcb_to_memoria(pcb , socket_memoria, ACTION);
	//recive_information_from_memoria(pcb , socket_memoria);
	return pcb;
}

void create_check_instructions_thread(t_args_check_instructions* args){
	pthread_t hilo_check_instructions;
	pthread_create(&hilo_check_instructions, NULL, check_state_of_pcb, args);
	pthread_detach(hilo_check_instructions);
}
