################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../json/json_reader.cpp \
../json/json_value.cpp \
../json/json_writer.cpp 

C_SRCS += \
../json/jsmn.c 

OBJS += \
./json/jsmn.o \
./json/json_reader.o \
./json/json_value.o \
./json/json_writer.o 

C_DEPS += \
./json/jsmn.d 

CPP_DEPS += \
./json/json_reader.d \
./json/json_value.d \
./json/json_writer.d 


# Each subdirectory must supply rules for building sources it contributes
json/%.o: ../json/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O2 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

json/%.o: ../json/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O2 -g -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


