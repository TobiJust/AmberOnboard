################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Child.cpp \
../src/Module.cpp \
../src/ModuleConfiguration.cpp \
../src/ModuleIO.cpp \
../src/ModuleImgProcessing.cpp \
../src/ModuleNetworking.cpp \
../src/ModuleSensors.cpp \
../src/Opt.cpp \
../src/ValContainer.cpp \
../src/Value.cpp \
../src/main.cpp 

OBJS += \
./src/Child.o \
./src/Module.o \
./src/ModuleConfiguration.o \
./src/ModuleIO.o \
./src/ModuleImgProcessing.o \
./src/ModuleNetworking.o \
./src/ModuleSensors.o \
./src/Opt.o \
./src/ValContainer.o \
./src/Value.o \
./src/main.o 

CPP_DEPS += \
./src/Child.d \
./src/Module.d \
./src/ModuleConfiguration.d \
./src/ModuleIO.d \
./src/ModuleImgProcessing.d \
./src/ModuleNetworking.d \
./src/ModuleSensors.d \
./src/Opt.d \
./src/ValContainer.d \
./src/Value.d \
./src/main.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -DTRUE=1 -DFALSE=0 -DDEBUG_MODE -I/usr/local/include/opencv -O0 -g3 -Wall -c -fmessage-length=0 -pthread -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


