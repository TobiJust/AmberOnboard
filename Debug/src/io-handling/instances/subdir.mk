################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/io-handling/instances/IOfile.cpp \
../src/io-handling/instances/IOi2cBus.cpp \
../src/io-handling/instances/IOparent.cpp \
../src/io-handling/instances/IOserial.cpp 

OBJS += \
./src/io-handling/instances/IOfile.o \
./src/io-handling/instances/IOi2cBus.o \
./src/io-handling/instances/IOparent.o \
./src/io-handling/instances/IOserial.o 

CPP_DEPS += \
./src/io-handling/instances/IOfile.d \
./src/io-handling/instances/IOi2cBus.d \
./src/io-handling/instances/IOparent.d \
./src/io-handling/instances/IOserial.d 


# Each subdirectory must supply rules for building sources it contributes
src/io-handling/instances/%.o: ../src/io-handling/instances/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -DTRUE=1 -DFALSE=0 -DDEBUG_MODE -I/usr/local/include/opencv -O0 -g3 -Wall -c -fmessage-length=0 -pthread -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


