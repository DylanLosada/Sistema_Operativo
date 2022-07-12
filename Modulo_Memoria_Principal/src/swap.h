#ifndef SWAP_H_
#define SWAP_H_

int eliminar_archivo_swap(t_memoria* memoria, t_pcb* pcb_proceso);
char* obtener_path_swap_del_archivo_del_proceso(int pcb_id, t_memoria* memoria);
void hacer_swap_del_proceso(t_pcb* pcb_proceso, t_memoria* memoria);
t_tabla_paginas_segundo_nivel* obtener_tabla_segundo_nivel_del_proceso(int id_tabla_pagina_segundo_nivel, t_memoria* memoria);
void hacer_swap_de_tabla_de_paginas_de_segundo_nivel(t_tabla_entradas_primer_nivel* tabla_primer_nivel, t_tabla_paginas_segundo_nivel* tabla_pagina_segundo_nivel_iteracion, FILE* archivo_proceso, t_memoria* memoria);
void hacer_swap_de_pagina_inicio(t_tabla_entradas_primer_nivel* tabla_primer_nivel, t_pagina_segundo_nivel* pagina_iteracion, int id_tabla_segundo_nivel, FILE* archivo_proceso, t_memoria* memoria);
void pasar_marco_ocupado_a_marco_libre_global(t_tabla_entradas_primer_nivel* tabla_primer_nivel, t_memoria* memoria);
void agregar_frames_libres_del_proceso_a_lista_global(t_tabla_entradas_primer_nivel* tabla_primer_nivel, t_memoria* memoria);
int tamanio_actual_del_archivo(FILE* archivo_proceso);
void swapear_pagina_en_disco(int pcb_id, t_memoria* memoria, t_marco* marco, t_pagina_segundo_nivel* pagina_a_agregar);
#endif
