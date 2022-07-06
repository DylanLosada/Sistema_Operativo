#ifndef SRC_CLOCK_H_
#define SRC_CLOCK_H_

#include "memoria.h"
#include "funciones_memoria.h"

void asignar_frame_a_pagina(t_memoria* memoria, t_tabla_entradas_primer_nivel* tabla_1er_nivel, t_pagina_segundo_nivel* pagina);
void clock(t_memoria* memoria, t_tabla_entradas_primer_nivel* tabla_1er_nivel, t_pagina_segundo_nivel* pagina_sin_frame);

#endif /* SRC_CLOCK_H_ */
