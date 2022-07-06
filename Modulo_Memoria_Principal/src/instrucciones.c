#include "instrucciones.h"

int leer_memoria(t_memoria* memoria, int direccion) {
	int valor;
	memcpy(&valor, memoria->espacio_memoria + direccion, sizeof(int));
	return valor;
}

void escribir_memoria(t_memoria* memoria, int direccion, int valor) {
	memcpy(memoria->espacio_memoria + direccion, &valor, sizeof(int));
}

int copiar_memoria(t_memoria* memoria, int direccion_desde, int direccion_hacia) {
	int valor = leer_memoria(memoria, direccion_desde);
	escribir_memoria(memoria, direccion_hacia, valor);
	return valor;
}

int get_tabla_segundo_nivel(t_memoria* memoria, int id_tabla_primer_nivel, int entrada) {
	t_tabla_entradas_primer_nivel* tabla = list_get(memoria->tablas_primer_nivel, id_tabla_primer_nivel); // TODO: quizas haya que buscar, ya que el index se mueve.
	int id_tabla_segundo_nivel = list_get(tabla->entradas, entrada);
	return id_tabla_segundo_nivel;
}

int get_marco(t_memoria* memoria, int id_tabla_segundo_nivel, int entrada) {
	t_tabla_paginas_segundo_nivel* tabla = list_get(memoria->tablas_segundo_nivel, id_tabla_segundo_nivel); // TODO: quizas haya que buscar, ya que el index se mueve.
	t_pagina_segundo_nivel* pagina = list_get(tabla->paginas_segundo_nivel, entrada);
	return pagina->marco_usado->numero_marco;
}
