################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/clock.c \
../src/funciones_memoria.c \
../src/instrucciones.c \
../src/memoria.c \
../src/swap.c 

OBJS += \
./src/clock.o \
./src/funciones_memoria.o \
./src/instrucciones.o \
./src/memoria.o \
./src/swap.o 

C_DEPS += \
./src/clock.d \
./src/funciones_memoria.d \
./src/instrucciones.d \
./src/memoria.d \
./src/swap.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

