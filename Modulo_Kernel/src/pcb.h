#ifndef PCB_H_
#define PCB_H_

#include "commons/collections/list.h"

typedef struct {
    int processSize;
    t_list* instructions;
    int pcb_id;
} t_pre_pcb;

#endif /* PCB_H_ */
