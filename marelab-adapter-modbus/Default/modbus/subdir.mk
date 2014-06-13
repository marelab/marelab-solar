################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../modbus/MBD.cpp \
../modbus/marelab_bus.cpp 

C_SRCS += \
../modbus/modbus-data.c \
../modbus/modbus-rtu.c \
../modbus/modbus-tcp.c \
../modbus/modbus.c 

OBJS += \
./modbus/MBD.o \
./modbus/marelab_bus.o \
./modbus/modbus-data.o \
./modbus/modbus-rtu.o \
./modbus/modbus-tcp.o \
./modbus/modbus.o 

C_DEPS += \
./modbus/modbus-data.d \
./modbus/modbus-rtu.d \
./modbus/modbus-tcp.d \
./modbus/modbus.d 

CPP_DEPS += \
./modbus/MBD.d \
./modbus/marelab_bus.d 


# Each subdirectory must supply rules for building sources it contributes
modbus/%.o: ../modbus/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"/mnt/marelab/marelab-nucleus/src/globals" -O2 -g -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

modbus/%.o: ../modbus/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O2 -g -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


