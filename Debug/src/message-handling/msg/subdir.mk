################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/message-handling/msg/Msg.cpp \
../src/message-handling/msg/MsgConfig.cpp \
../src/message-handling/msg/MsgTerminalInput.cpp 

OBJS += \
./src/message-handling/msg/Msg.o \
./src/message-handling/msg/MsgConfig.o \
./src/message-handling/msg/MsgTerminalInput.o 

CPP_DEPS += \
./src/message-handling/msg/Msg.d \
./src/message-handling/msg/MsgConfig.d \
./src/message-handling/msg/MsgTerminalInput.d 


# Each subdirectory must supply rules for building sources it contributes
src/message-handling/msg/%.o: ../src/message-handling/msg/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -DTRUE=1 -DFALSE=0 -DDEBUG_MODE -I/usr/local/include/opencv -O0 -g3 -Wall -c -fmessage-length=0 -pthread -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


