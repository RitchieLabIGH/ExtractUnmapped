################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Process/Reduce.cpp 

OBJS += \
./src/Process/Reduce.o 

CPP_DEPS += \
./src/Process/Reduce.d 


# Each subdirectory must supply rules for building sources it contributes
src/Process/%.o: ../src/Process/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++14 -O3 -Wall -c -fmessage-length=0 -fopenmp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


