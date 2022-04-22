#ifndef KERNEL_H_
#define KERNEL_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/log.h>
#include<commons/config.h>

#include "sockets.h"
#include "conexion.h"



void iterator(char* value);
t_config* iniciar_config(void);

#endif /* KERNEL_H_ */
