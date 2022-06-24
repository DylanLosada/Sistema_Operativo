H_SRCS=$(shell find . -iname "*.h" | tr '\n' ' ')

uninstall:
	sudo rm -f /usr/lib/libShared.so
	sudo rm -rf /usr/include/shared_funtions
	
install:
	-cd /home/utnso/tp-2022-1c-SanguchitOS/Shared/Debug/ && $(MAKE) all
	-cp -u /home/utnso/tp-2022-1c-SanguchitOS/Shared/Debug/libShared.so /usr/lib
	-cp --parents -u $(H_SRCS) /usr/include
	-cd /home/utnso/tp-2022-1c-SanguchitOS/Modulo_Kernel/Debug/ && $(MAKE) all
	-cd /home/utnso/tp-2022-1c-SanguchitOS/Modulo_Consola/Default/ && $(MAKE) all
	-cd /home/utnso/tp-2022-1c-SanguchitOS/Modulo_CPU/Debug/ && $(MAKE) all
	-cd /home/utnso/tp-2022-1c-SanguchitOS/Modulo_Memoria_Principal/Debug/ && $(MAKE) all