#include "check_interrupt.h"

void recive_interrupt(t_interrupt_message* exist_interrupt){

	if(exist_interrupt->socket > 0){

		int op_code;

		while(1){
			recv(exist_interrupt->socket, &op_code, sizeof(int), MSG_WAITALL);
			// VER QUE PASA SI ES MENOR QUE 0
			// SEMAFOROS
			*exist_interrupt->is_interrupt = true;
		}
	}


}

