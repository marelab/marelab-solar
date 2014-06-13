################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/cgicc/CgiEnvironment.cpp \
../src/cgicc/CgiInput.cpp \
../src/cgicc/CgiUtils.cpp \
../src/cgicc/Cgicc.cpp \
../src/cgicc/FormEntry.cpp \
../src/cgicc/FormFile.cpp \
../src/cgicc/HTMLAttribute.cpp \
../src/cgicc/HTMLAttributeList.cpp \
../src/cgicc/HTMLDoctype.cpp \
../src/cgicc/HTMLElement.cpp \
../src/cgicc/HTMLElementList.cpp \
../src/cgicc/HTTPContentHeader.cpp \
../src/cgicc/HTTPCookie.cpp \
../src/cgicc/HTTPHTMLHeader.cpp \
../src/cgicc/HTTPHeader.cpp \
../src/cgicc/HTTPPlainHeader.cpp \
../src/cgicc/HTTPRedirectHeader.cpp \
../src/cgicc/HTTPResponseHeader.cpp \
../src/cgicc/HTTPStatusHeader.cpp \
../src/cgicc/HTTPXHTMLHeader.cpp \
../src/cgicc/MStreamable.cpp \
../src/cgicc/XHTMLDoctype.cpp \
../src/cgicc/XMLPI.cpp 

OBJS += \
./src/cgicc/CgiEnvironment.o \
./src/cgicc/CgiInput.o \
./src/cgicc/CgiUtils.o \
./src/cgicc/Cgicc.o \
./src/cgicc/FormEntry.o \
./src/cgicc/FormFile.o \
./src/cgicc/HTMLAttribute.o \
./src/cgicc/HTMLAttributeList.o \
./src/cgicc/HTMLDoctype.o \
./src/cgicc/HTMLElement.o \
./src/cgicc/HTMLElementList.o \
./src/cgicc/HTTPContentHeader.o \
./src/cgicc/HTTPCookie.o \
./src/cgicc/HTTPHTMLHeader.o \
./src/cgicc/HTTPHeader.o \
./src/cgicc/HTTPPlainHeader.o \
./src/cgicc/HTTPRedirectHeader.o \
./src/cgicc/HTTPResponseHeader.o \
./src/cgicc/HTTPStatusHeader.o \
./src/cgicc/HTTPXHTMLHeader.o \
./src/cgicc/MStreamable.o \
./src/cgicc/XHTMLDoctype.o \
./src/cgicc/XMLPI.o 

CPP_DEPS += \
./src/cgicc/CgiEnvironment.d \
./src/cgicc/CgiInput.d \
./src/cgicc/CgiUtils.d \
./src/cgicc/Cgicc.d \
./src/cgicc/FormEntry.d \
./src/cgicc/FormFile.d \
./src/cgicc/HTMLAttribute.d \
./src/cgicc/HTMLAttributeList.d \
./src/cgicc/HTMLDoctype.d \
./src/cgicc/HTMLElement.d \
./src/cgicc/HTMLElementList.d \
./src/cgicc/HTTPContentHeader.d \
./src/cgicc/HTTPCookie.d \
./src/cgicc/HTTPHTMLHeader.d \
./src/cgicc/HTTPHeader.d \
./src/cgicc/HTTPPlainHeader.d \
./src/cgicc/HTTPRedirectHeader.d \
./src/cgicc/HTTPResponseHeader.d \
./src/cgicc/HTTPStatusHeader.d \
./src/cgicc/HTTPXHTMLHeader.d \
./src/cgicc/MStreamable.d \
./src/cgicc/XHTMLDoctype.d \
./src/cgicc/XMLPI.d 


# Each subdirectory must supply rules for building sources it contributes
src/cgicc/%.o: ../src/cgicc/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	arm-linux-gnueabihf-g++ -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


