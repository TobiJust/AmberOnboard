################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/network-handling/FrameProcessor.cpp \
../src/network-handling/NW_SocketInterface.cpp \
../src/network-handling/ProcDataFrame.cpp \
../src/network-handling/ProcPayload.cpp 

OBJS += \
./src/network-handling/FrameProcessor.o \
./src/network-handling/NW_SocketInterface.o \
./src/network-handling/ProcDataFrame.o \
./src/network-handling/ProcPayload.o 

CPP_DEPS += \
./src/network-handling/FrameProcessor.d \
./src/network-handling/NW_SocketInterface.d \
./src/network-handling/ProcDataFrame.d \
./src/network-handling/ProcPayload.d 


# Each subdirectory must supply rules for building sources it contributes
src/network-handling/%.o: ../src/network-handling/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -DTRUE=1 -DFALSE=0 -DDEBUG_MODE -I/usr/local/include/opencv -O0 -g3 -Wall -c -fmessage-length=0 -pthread -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


