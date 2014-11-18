################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/io-handling/IOHandler.cpp \
../src/io-handling/StrProcessor.cpp \
../src/io-handling/StreamIO.cpp \
../src/io-handling/Terminal.cpp 

OBJS += \
./src/io-handling/IOHandler.o \
./src/io-handling/StrProcessor.o \
./src/io-handling/StreamIO.o \
./src/io-handling/Terminal.o 

CPP_DEPS += \
./src/io-handling/IOHandler.d \
./src/io-handling/StrProcessor.d \
./src/io-handling/StreamIO.d \
./src/io-handling/Terminal.d 


# Each subdirectory must supply rules for building sources it contributes
src/io-handling/%.o: ../src/io-handling/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -DTRUE=1 -DFALSE=0 -DDEBUG_MODE -I/usr/local/include/opencv -O0 -g3 -Wall -c -fmessage-length=0 -pthread -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


