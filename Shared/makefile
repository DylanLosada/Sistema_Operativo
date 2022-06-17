H_SRCS=$(shell find . -iname "*.h" | tr '\n' ' ')

install:
	-cd /home/utnso/tp-2022-1c-SanguchitOS/Modulo_Kernel/Debug/ && $(MAKE) all
	-cd /home/utnso/tp-2022-1c-SanguchitOS/Modulo_Consola/Default/ && $(MAKE) all
	-cd /home/utnso/tp-2022-1c-SanguchitOS/Modulo_CPU/Debug/ && $(MAKE) all
	#-cd /home/utnso/tp-2022-1c-SanguchitOS/Modulo_Memoria/Debug/ && $(MAKE) all
	-cd /home/utnso/tp-2022-1c-SanguchitOS/Shared/Debug/ && $(MAKE) all
	-sudo cp -u /home/utnso/tp-2022-1c-SanguchitOS/Shared/Debug/libShared.so /usr/lib
	-sudo cp --parents -u $(H_SRCS) /usr/include