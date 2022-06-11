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
	char* planner_type;
	t_queue* pre_pcbs;
	t_config_kernel* config_kernel;
	pthread_mutex_t* mutex;
	pthread_mutex_t* hasNewConsole;
} t_args_planificador;


typedef struct{
	int check_state_instructions;
	int interrupt;
	int dispatch;
} t_sockets_cpu;

typedef struct{
	int socket;
	t_pcb* pcb;
	bool isExitInstruction;
	bool hasUpdateState;
	pthread_mutex_t* mutex_check_instruct;
	pthread_mutex_t* hasPcb;
	pthread_mutex_t* hasPcbRunning;
} t_args_check_instructions;

typedef struct {
	t_queue* state;
	pthread_mutex_t* mutex;
} t_state_queue_hanndler;

typedef struct {
	t_list* state;
	pthread_mutex_t* mutex;
} t_state_list_hanndler;

typedef struct{
	int gradoMultiprogramacionActual;
	pthread_mutex_t* mutex;
} t_monitor_grado_multiprogramacion;

typedef struct{
	t_state_queue_hanndler* state_new;
	t_state_queue_hanndler* state_exit;
	t_state_queue_hanndler* state_running;
	t_state_list_hanndler* state_ready;
	t_state_list_hanndler* state_suspended_blocked;
	t_state_list_hanndler* state_suspended_ready;
	t_state_list_hanndler* state_blocked;
} t_states;

typedef struct{
	int GRADO_MULTIPROGRAMACION;
	int ESTIMACION_INICIAL;
	int socket_memoria;
	bool* isFirstPcb;
	pthread_mutex_t* pre_pcbs_mutex;
	pthread_mutex_t* hasNewConsole;
	pthread_mutex_t* hasPcb;
	t_monitor_grado_multiprogramacion* monitorGradoMulti;
	t_queue* pre_pcbs;
	t_states* states;
} t_args_long_term_planner;

typedef struct{
	int TIEMPO_MAXIMO_BLOQUEADO;
	int socket_memoria;
	pthread_mutex_t* hasPcbBlocked;
	t_monitor_grado_multiprogramacion* monitorGradoMulti;
	t_states* states;
} t_args_mid_term_planner;

typedef struct{
	int ALFA;
	int GRADO_MULTIPROGRAMACION;
	int TIEMPO_MAXIMO_BLOQUEADO;
	int socket_memoria;
	char* ALGORITMO_PLANIFICACION;
	pthread_mutex_t* hasPcb;
	pthread_mutex_t* hasPcbBlocked;
	pthread_mutex_t* hasPcbRunning;
	t_monitor_grado_multiprogramacion* monitorGradoMulti;
	t_sockets_cpu* sockets_cpu;
	t_config_kernel* config_kernel;
	t_states* states;
} t_args_short_term_planner;

void long_term_planner(void* args_long_term_planner);
void mid_term_planner(void* args_mid_term_planner);
void short_term_planner(void* args_short_planner);
void order_queue(char* planner_type, t_queue* queue_to_oreder);
void handler_planners(void* void_args);
int connect_to_interrupt_cpu(t_config_kernel* config_kernel);
bool hasCalculateRafaga(t_pcb* pcb);
bool hasRunningPcb(t_queue* state_ready);
bool isNewPcbIntoReady(int pre_evaluate_add_pcb_to_ready_size, t_list* state_ready);
t_pcb* create_pcb(t_monitor_grado_multiprogramacion* monitorGradoMulti, int GRADO_MULTIPROGRAMACION, int ESTIMACION_INICIAL, bool* isFirstPcb, t_pre_pcb* pre_pcb);
int interrupt_cpu(int socket_kernel_interrupt_cpu, op_code INTERRUPT, t_pcb* pcb_excecuted);
t_pcb* send_action_to_memoria(t_pcb* pcb, int socket_memoria, int ACTION);
void check_time_in_blocked_and_pass_to_suspended_blocked(t_state_list_hanndler* state_suspended_blocked, t_state_list_hanndler* state_blocked, t_monitor_grado_multiprogramacion* monitorGradoMulti, int socket_memoria, int TIEMPO_MAXIMO_BLOQUEADO);

#endif;
