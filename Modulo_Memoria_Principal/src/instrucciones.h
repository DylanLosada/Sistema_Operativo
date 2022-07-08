#ifndef SRC_INSTRUCCIONES_H_
#define SRC_INSTRUCCIONES_H_

#include "memoria.h"
#include "funciones_memoria.h"

int leer_memoria(t_memoria* memoria, int direccion);
void escribir_memoria(t_memoria* memoria, int direccion, int valor);
int copiar_memoria(t_memoria* memoria, int direccion_desde, int direccion_hacia);

int get_tabla_segundo_nivel(t_memoria* memoria, int tabla_primer_nivel, int entrada);
int get_marco(t_memoria* memoria, int tabla_segundo_nivel, int entrada, int* marco_to_swap, op_memoria_message INSTRUCCION);

#endif /* SRC_INSTRUCCIONES_H_ */
