#ifndef SRC_CPU_H_
#define SRC_CPU_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/collections/list.h>

#include "pcb.h"

#include "fetch.h"
#include "decode.h"
#include "fetch_operands.h"
#include "execute.h"
#include "check_interrupt.h"


t_config* iniciar_config(void);

#endif
