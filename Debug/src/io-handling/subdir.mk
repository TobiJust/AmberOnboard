################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/io-handling/Config.cpp \
../src/io-handling/GpsAdafruit.cpp \
../src/io-handling/MPU6050.cpp \
../src/io-handling/NmeaParser.cpp \
../src/io-handling/Terminal.cpp 

OBJS += \
./src/io-handling/Config.o \
./src/io-handling/GpsAdafruit.o \
./src/io-handling/MPU6050.o \
./src/io-handling/NmeaParser.o \
./src/io-handling/Terminal.o 

CPP_DEPS += \
./src/io-handling/Config.d \
./src/io-handling/GpsAdafruit.d \
./src/io-handling/MPU6050.d \
./src/io-handling/NmeaParser.d \
./src/io-handling/Terminal.d 


# Each subdirectory must supply rules for building sources it contributes
src/io-handling/%.o: ../src/io-handling/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -DTRUE=1 -DFALSE=0 -DDEBUG_MODE -I/usr/local/include/opencv -O0 -g3 -Wall -c -fmessage-length=0 -pthread -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


