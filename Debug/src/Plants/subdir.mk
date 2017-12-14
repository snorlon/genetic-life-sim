################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Plants/PlantSimple.cpp 

OBJS += \
./src/Plants/PlantSimple.o 

CPP_DEPS += \
./src/Plants/PlantSimple.d 


# Each subdirectory must supply rules for building sources it contributes
src/Plants/%.o: ../src/Plants/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	gcc -I/usr/local/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


