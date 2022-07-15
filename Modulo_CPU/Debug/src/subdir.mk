################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/config_cpu.c \
../src/cpu.c \
../src/execute.c \
../src/fetch_and_decode.c \
../src/funciones.c \
../src/mmu.c \
../src/tlb.c 

OBJS += \
./src/config_cpu.o \
./src/cpu.o \
./src/execute.o \
./src/fetch_and_decode.o \
./src/funciones.o \
./src/mmu.o \
./src/tlb.o 

C_DEPS += \
./src/config_cpu.d \
./src/cpu.d \
./src/execute.d \
./src/fetch_and_decode.d \
./src/funciones.d \
./src/mmu.d \
./src/tlb.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


