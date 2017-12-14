################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/MyLittleWorld.cpp \
../src/Organism.cpp \
../src/PopulationManager.cpp 

OBJS += \
./src/MyLittleWorld.o \
./src/Organism.o \
./src/PopulationManager.o 

CPP_DEPS += \
./src/MyLittleWorld.d \
./src/Organism.d \
./src/PopulationManager.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	gcc -I/usr/local/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


