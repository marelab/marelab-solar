################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../LedString.cpp \
../LedTimerListe.cpp \
../led-dimmer-plugin.cpp 

OBJS += \
./LedString.o \
./LedTimerListe.o \
./led-dimmer-plugin.o 

CPP_DEPS += \
./LedString.d \
./LedTimerListe.d \
./led-dimmer-plugin.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O2 -g -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


