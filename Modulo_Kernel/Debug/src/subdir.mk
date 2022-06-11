################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/conexion.c \
../src/config_kernel.c \
../src/funciones.c \
../src/kernel.c \
../src/pcb.c \
../src/procesamiento.c 

OBJS += \
./src/conexion.o \
./src/config_kernel.o \
./src/funciones.o \
./src/kernel.o \
./src/pcb.o \
./src/procesamiento.o 

C_DEPS += \
./src/conexion.d \
./src/config_kernel.d \
./src/funciones.d \
./src/kernel.d \
./src/pcb.d \
./src/procesamiento.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/tp-2022-1c-SanguchitOS/Shared/shared_funtions" -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


