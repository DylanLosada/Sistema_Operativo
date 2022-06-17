################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../shared_funtions/conexion_cliente.c \
../shared_funtions/conexion_servidor.c \
../shared_funtions/estructuras.c \
../shared_funtions/free_memory.c 

OBJS += \
./shared_funtions/conexion_cliente.o \
./shared_funtions/conexion_servidor.o \
./shared_funtions/estructuras.o \
./shared_funtions/free_memory.o 

C_DEPS += \
./shared_funtions/conexion_cliente.d \
./shared_funtions/conexion_servidor.d \
./shared_funtions/estructuras.d \
./shared_funtions/free_memory.d 


# Each subdirectory must supply rules for building sources it contributes
shared_funtions/%.o: ../shared_funtions/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


