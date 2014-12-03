################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/nw-handling/FrameProcessor.cpp \
../src/nw-handling/NW_SocketInterface.cpp \
../src/nw-handling/NetworkCommunicator.cpp \
../src/nw-handling/ProcDataFrame.cpp \
../src/nw-handling/ProcPayload.cpp 

OBJS += \
./src/nw-handling/FrameProcessor.o \
./src/nw-handling/NW_SocketInterface.o \
./src/nw-handling/NetworkCommunicator.o \
./src/nw-handling/ProcDataFrame.o \
./src/nw-handling/ProcPayload.o 

CPP_DEPS += \
./src/nw-handling/FrameProcessor.d \
./src/nw-handling/NW_SocketInterface.d \
./src/nw-handling/NetworkCommunicator.d \
./src/nw-handling/ProcDataFrame.d \
./src/nw-handling/ProcPayload.d 


# Each subdirectory must supply rules for building sources it contributes
src/nw-handling/%.o: ../src/nw-handling/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -DTRUE=1 -DFALSE=0 -DDEBUG_MODE -I/usr/local/include/opencv -O0 -g3 -Wall -c -fmessage-length=0 -pthread -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


