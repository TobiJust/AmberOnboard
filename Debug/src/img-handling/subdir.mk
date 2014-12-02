################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/img-handling/Container.cpp \
../src/img-handling/ImgCapture.cpp \
../src/img-handling/ImgOpExecutor.cpp \
../src/img-handling/ImgOperator.cpp \
../src/img-handling/OpComposite.cpp \
../src/img-handling/OpEncodeJPEG.cpp \
../src/img-handling/OpPictureInPicture.cpp \
../src/img-handling/OpPrepare.cpp 

OBJS += \
./src/img-handling/Container.o \
./src/img-handling/ImgCapture.o \
./src/img-handling/ImgOpExecutor.o \
./src/img-handling/ImgOperator.o \
./src/img-handling/OpComposite.o \
./src/img-handling/OpEncodeJPEG.o \
./src/img-handling/OpPictureInPicture.o \
./src/img-handling/OpPrepare.o 

CPP_DEPS += \
./src/img-handling/Container.d \
./src/img-handling/ImgCapture.d \
./src/img-handling/ImgOpExecutor.d \
./src/img-handling/ImgOperator.d \
./src/img-handling/OpComposite.d \
./src/img-handling/OpEncodeJPEG.d \
./src/img-handling/OpPictureInPicture.d \
./src/img-handling/OpPrepare.d 


# Each subdirectory must supply rules for building sources it contributes
src/img-handling/%.o: ../src/img-handling/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -DTRUE=1 -DFALSE=0 -DDEBUG_MODE -I/usr/local/include/opencv -O0 -g3 -Wall -c -fmessage-length=0 -pthread -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


