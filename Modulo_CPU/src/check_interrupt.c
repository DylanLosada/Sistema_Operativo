#include "check_interrupt.h"

void recive_interrupt(t_interrupt_message* exist_interrupt){

	if(exist_interrupt->socket > 0){\

		char* message;

		while(1){
			recv(exist_interrupt->socket, message, sizeof(char*), MSG_WAITALL);
			exist_interrupt->is_interrupt = true;
		}
	}


}

