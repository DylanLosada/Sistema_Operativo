################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/conexion.c \
../src/kernel.c \
../src/pcb.c \
../src/procesamiento.c \
../src/protocolo.c \
../src/sockets.c 

OBJS += \
./src/conexion.o \
./src/kernel.o \
./src/pcb.o \
./src/procesamiento.o \
./src/protocolo.o \
./src/sockets.o 

C_DEPS += \
./src/conexion.d \
./src/kernel.d \
./src/pcb.d \
./src/procesamiento.d \
./src/protocolo.d \
./src/sockets.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


