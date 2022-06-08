#ifndef PLANIFICADORES_H_
#define PLANIFICADORES_H_

#include "../conexion.h"
#include <stdbool.h>
#include <time.h>

typedef enum{
	FIFO,
	SRT
} tipo_planificador_enum;

typedef struct{
	t_queue* pre_pcbs;
	char* planner_type;
	t_config_kernel* config_kernel;
	pthread_mutex_t* mutex;
} t_args_planificador;


typedef struct{
	int check_state_instructions;
	int interrupt;
	int dispatch;
} t_sockets_cpu;

typedef struct{
	int socket;
	t_pcb* pcb;
	pthread_mutex_t* mutex_check_instruct;
	bool isExitInstruction;
	bool hasUpdateState;
} t_args_check_instructions;

typedef struct{
	t_queue* state_new;
	t_queue* state_exit;
	t_list* state_ready;
	t_queue* state_running;
	t_list* state_suspended_blocked;
	t_list* state_suspended_ready;
	t_list* state_blocked;
} t_states;

void long_term_planner(bool* isFirstPcb, pthread_mutex_t* mutex, t_queue* pre_pbcs, t_states* states);
void mid_term_planner(int TIEMPO_MAXIMO_BLOQUEADO, t_states* states);
void order_queue(char* planner_type, t_queue* queue_to_oreder);
void handler_planners(void* void_args);
int connect_to_interrupt_cpu(t_config_kernel* config_kernel);
bool hasCalculateRafaga(t_pcb* pcb);
bool hasRunningPcb(t_queue* state_ready);
bool isNewPcbIntoReady(int pre_evaluate_add_pcb_to_ready_size, t_list* state_ready);
t_pcb* create_pcb(bool* isFirstPcb, t_pre_pcb* pre_pcb);
int interrupt_cpu(int socket_kernel_interrupt_cpu, op_code INTERRUPT, t_pcb* pcb_excecuted);

#endif;
