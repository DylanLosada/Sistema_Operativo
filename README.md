<!-- @format -->

# SISTEMA OPERATIVO: SANGUCHITOS

Proyecto desarrollado para la materia Sistemas Operativos, donde se desarrollo el sistema solicitado en el siguiente [documento](https://docs.google.com/document/d/17WP76Vsi6ZrYlpYT8xOPXzLf42rQgtyKsOdVkyL5Jj0/edit).

## FUNCIONAMIENTO

El sistema está formado por un [Kernel](#id1), una [CPU](#id2), una [Memoria RAM](#id3) y una indefinida cantidad de [Consolas](#id4).
Las conexiones están realizas mediante el protocolo TCP utilizando sockets y el modelo de Cliente-Servidos.
Para poder levantar el proyecto entero se debe editar la carpeta: configs/. Donde, dependiendo de que prueba se desea realizar, se deberán configurar las IPs de los distintos módulos.

#### Consolas<a name="id4"></a>

Representación formal de los procesos en nuestro sistema. La consola recibe tres parámetros: tamaño del proceso (en bytes), ruta del archivo de configuración (debe ser absoluta) y archivo de instrucciones (debe ser una ruta absoluta).

#### Kernel<a name="id1"></a>

El kernel realiza la planificación de los procesos, en nuestro caso serian las consolas conectadas. Esta planificación es partida en cada uno de los distintos planificadores que posee el kernel: Largo, Medio y Corto Plazo.
El Largo Plazo es el encargado de aceptar nuevos procesos, si es que existe grado de multiprogramación disponible, y eliminarlos del sistema, cuando el proceso termine su ejecución.
El Medio Plazo se encarga de liberar el grado de multiprogramación mediante la suspensión de los procesos que superaron un determinado tiempo bloqueados.
El Corto Plazo es el encargado de ordenar los procesos a ejecutar, mediante un determinado algoritmo (FIFO o SRT), y enviar los procesos a ejecutar (los pasa a la CPU).
El kernel recibe un parámetros: ruta del archivo de configuración (debe ser absoluta).

#### CPU<a name="id2"></a>

La CPU es la encargada de realizar las instrucciones que posee cada proceso, para esto realiza las traducciones de las direcciones lógicas en físicas y las envía a Memoria. Para el proceso de traducción cuenta con una TLB que le permitirá, mediante ciertos HITS, poder reducir los accesos a memoria realizados. En caso de que las instrucciones sean una Salida o una I/O, la CPU le devolverá al Kernel el proceso.
La CPU recibe un parámetros: ruta del archivo de configuración (debe ser absoluta).

#### Memoria RAM<a name="id3"></a>

La Memoria es la encargada del almacenamiento, durante el tiempo de vida del proceso, de las tablas de páginas, así como del SWAP realizado por los procesos suspendidos.
La Memoria recibe un parámetros: ruta del archivo de configuración (debe ser absoluta).

## PRUEBAS

Las pruebas consistieron en establecer un cierto nivel de estrés en el sistema, donde cada una de ellas apuntaba a testear una característica específica del sistema. Se pueden ver la pruebas realizadas en el siguiente [documento de pruebas](https://docs.google.com/document/d/1SBBTCweMCiBg6TPTt7zxdinRh4ealRasbu0bVlkty5o/edit). Por otro lado, los archivos de las pruebas: [repositorio de pruebas](https://github.com/sisoputnfrba/kiss-pruebas).
