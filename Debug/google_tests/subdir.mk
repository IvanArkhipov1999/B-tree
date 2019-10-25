################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../google_tests/Bplustree.cpp \
../google_tests/tests.cpp 

OBJS += \
./google_tests/Bplustree.o \
./google_tests/tests.o 

CPP_DEPS += \
./google_tests/Bplustree.d \
./google_tests/tests.d 


# Each subdirectory must supply rules for building sources it contributes
google_tests/%.o: ../google_tests/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


