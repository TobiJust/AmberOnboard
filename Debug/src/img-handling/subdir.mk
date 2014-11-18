################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/img-handling/ImgCapture.cpp \
../src/img-handling/ImgOpExecutor.cpp \
../src/img-handling/ImgOperator.cpp 

OBJS += \
./src/img-handling/ImgCapture.o \
./src/img-handling/ImgOpExecutor.o \
./src/img-handling/ImgOperator.o 

CPP_DEPS += \
./src/img-handling/ImgCapture.d \
./src/img-handling/ImgOpExecutor.d \
./src/img-handling/ImgOperator.d 


# Each subdirectory must supply rules for building sources it contributes
src/img-handling/%.o: ../src/img-handling/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -DTRUE=1 -DFALSE=0 -DDEBUG_MODE -I/usr/local/include/opencv -O0 -g3 -Wall -c -fmessage-length=0 -pthread -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


