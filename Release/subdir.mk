################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
INO_SRCS += \
../Driver.ino \
../MMjoystick.ino \
../MMloop.ino 

OBJS += \
./Driver.o \
./MMjoystick.o \
./MMloop.o 

INO_DEPS += \
./Driver.d \
./MMjoystick.d \
./MMloop.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.ino
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -I"/home/jon/src/arduino-1.0/hardware/arduino/cores/arduino" -I"/home/jon/src/arduino-1.0/hardware/arduino/variants/standard" -I"/home/jon/workspace/MMvector" -D__IN_ECLIPSE__=1 -DARDUINO=100 -Wall -Os -g -mmcu=atmega328p -DF_CPU=16000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o "$@" -x c++ "$<"
	@echo 'Finished building: $<'
	@echo ' '


