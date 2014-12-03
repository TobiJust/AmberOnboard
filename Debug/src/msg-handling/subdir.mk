################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/msg-handling/Msg.cpp \
../src/msg-handling/MsgConfig.cpp \
../src/msg-handling/MsgHub.cpp \
../src/msg-handling/Observer.cpp 

OBJS += \
./src/msg-handling/Msg.o \
./src/msg-handling/MsgConfig.o \
./src/msg-handling/MsgHub.o \
./src/msg-handling/Observer.o 

CPP_DEPS += \
./src/msg-handling/Msg.d \
./src/msg-handling/MsgConfig.d \
./src/msg-handling/MsgHub.d \
./src/msg-handling/Observer.d 


# Each subdirectory must supply rules for building sources it contributes
src/msg-handling/%.o: ../src/msg-handling/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -DTRUE=1 -DFALSE=0 -DDEBUG_MODE -I/usr/local/include/opencv -O0 -g3 -Wall -c -fmessage-length=0 -pthread -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


