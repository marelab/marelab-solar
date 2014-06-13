################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../json/json_reader.cpp \
../json/json_value.cpp \
../json/json_writer.cpp 

OBJS += \
./json/json_reader.o \
./json/json_value.o \
./json/json_writer.o 

CPP_DEPS += \
./json/json_reader.d \
./json/json_value.d \
./json/json_writer.d 


# Each subdirectory must supply rules for building sources it contributes
json/%.o: ../json/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"/mnt/marelab/marelab-nucleus/src/globals" -O2 -g -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


